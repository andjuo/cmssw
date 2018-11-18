#include "EventFilter/GEMRawToDigi/plugins/GEMLocalModeDataSource.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "EventFilter/GEMRawToDigi/interface/AMC13Event.h"
#include "EventFilter/GEMRawToDigi/interface/VFATdata.h"

#include <fstream>

GEMLocalModeDataSource::GEMLocalModeDataSource(const edm::ParameterSet & pset, edm::InputSourceDescription const &desc) :
  edm::ProducerSourceFromFiles(pset,desc,true), // true - RealData
  m_hasFerolHeader( pset.getUntrackedParameter<bool>("hasFerolHeader",false)),
  m_fedid( pset.getUntrackedParameter<int>("fedId", 12345)),
  m_fileindex(0),
  m_runnumber( pset.getUntrackedParameter<int>("runNumber",-1)),
  m_currenteventnumber(1),
  m_currenttriggernumber(0),
  m_eventnumber_shift(0)
{
  produces<FEDRawDataCollection>("gemLocalModeDataSource");

  std::cout << "there are " << this->fileNames().size() << " files: ";
  for (unsigned int i=0; i<this->fileNames().size(); i++) {
    std::cout << " " << this->fileNames().at(i);
  }
  std::cout << "\n";

  IOOffset size = -1;
  StorageFactory::getToModify()->enableAccounting(true);

  for (unsigned int i=0; i<this->fileNames().size(); i++) {
    std::string fname = this->fileNames().at(i);
    bool exists = StorageFactory::get()->check(fname, &size);
    std::cout << "file " << fname << " size " << size << std::endl;
    if (!exists) {
      std::stringstream ss;
      ss << "file not found (" << fname << ")";
      throw cms::Exception(ss.str());
    }
  }

  std::string currentfilename = this->fileNames()[m_fileindex];
  std::cout << "examining " << currentfilename << std::endl;
  m_fileindex++;

  if (0) {
    std::string fname= currentfilename.substr(5,currentfilename.size());
    std::cout << "trying to open the file " << fname << std::endl;
    std::ifstream fin(fname.c_str(),std::ios::binary);
    if (!fin.is_open()) {
      std::cout << "failed" << std::endl;
    }
    else {
      const int bufsize=150;
      uint64_t buf[bufsize];
      fin.read((char*)&buf,bufsize*sizeof(uint64_t));

      if (0)
	for (uint64_t i=0; i<bufsize*sizeof(uint64_t); i++) {
	  char c= ((char*)&buf)[i];
	  std::cout << "i=" << i << ", char=" << c << std::endl;
	}

      std::cout << "got words\n";
      gem::AMC13Event amc13Event;
      for (int i=0; i<bufsize-1; i++) {
	amc13Event.setCDFHeader(buf[i]);
	amc13Event.setAMC13Header(buf[i+1]);

	int ok=0;
	//if (buf[i] > ((uint64_t)(1)<<60)) std::cout << "b ";
	//else { std::cout << "g "; ok=1; }
	if (amc13Event.get_cb5()==5) ok=1;
	std::cout << ((ok) ? "g " : "b ");
	std::cout << "0x" << std::hex << buf[i] << std::dec << "  ";

	if (ok) {
	  std::cout << "cb5=" << amc13Event.get_cb5() << " cb0=" << amc13Event.get_cb0()
		    << " nAMC=" << amc13Event.get_nAMC();
	}

	std::cout << std::endl;
      }
    }
    fin.close();
  }

  // open file stream
  storage = StorageFactory::get()->open(currentfilename);
  // (throw if storage is 0)
  if (!storage) {
    std::stringstream ss;
    ss << "GEMLocalModeDataSource: failed to open the file";
    throw cms::Exception(ss.str());
  }

  m_runnumber = m_fileindex; // dummy run number
 }


GEMLocalModeDataSource::~GEMLocalModeDataSource()
{
}


void GEMLocalModeDataSource::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.setComment("Reads GEM data saved in 'local mode' to produce FEDRawDataCollection.");
  desc.addUntracked<std::vector<std::string> >("fileNames")
    ->setComment("Names of files to be processed.");
  desc.addUntracked<unsigned int>("skipEvents")
    ->setComment("Number of events to skip before next processing.");
  desc.addUntracked<bool>("hasFerolHeader", false)
    ->setComment("Whether additional header is present.");
  desc.addUntracked<int>("fedId", 12345)
    ->setComment("FedID value to embed into events.");
  desc.addUntracked<int>("runNumber", -1)
    ->setComment("Which runNumber to embed:\n -1 - get from filename,\n other - use this value.");

  //ProductSelectorRules::fillDescription(desc, "inputCommands");

  descriptions.addDefault(desc);
}


bool GEMLocalModeDataSource::setRunAndEventInfo(edm::EventID &id, edm::TimeValue_t &time, edm::EventAuxiliary::ExperimentType &)
{
  std::cout << "\nsetRunAndEventInfo m_fileindex=" << m_fileindex << std::endl;

  if (storage->eof()) {
    storage->close();

    if (m_fileindex >= this->fileNames().size()) {
      std::cout << "end of last file" << std::endl;
      return false;
    }
    std::string currentfilename = this->fileNames()[m_fileindex];
    std::cout << "processing " << currentfilename << std::endl;
    m_fileindex++;
    storage = StorageFactory::get()->open(currentfilename);
    if (!storage) {
      std::cout << "failed to open next file" << std::endl;
      return false;
    }
  }

  Storage & inpFile = *storage;

  // create product (raw data)
  buffers.reset( new FEDRawDataCollection );

  // no loop over data, assuming record is 1 event
  m_currenteventnumber++;
  std::cout << "GEMLocalModeDataSource::setRunAndEventInfo m_currenteventnumber=" << m_currenteventnumber << std::endl;

  //std::cout << "the number= " << sizeof(uint64_t) << std::endl;
  std::vector<uint64_t> buf;
  const int tmpBufSize=24;
  uint64_t tmpBuf[tmpBufSize];

  // get CDFHeader and AMC13Header
  int n=inpFile.read((char*)tmpBuf,2*sizeof(uint64_t)); // number of bytes to read
  if (n!=2*sizeof(uint64_t)) {
    std::cout << "failed to read next 2 words" << std::endl;
    storage->close();
    return false;
  }

  std::cout << "got 1st data " << tmpBuf[0] << std::endl;
  buf.push_back(tmpBuf[0]);
  buf.push_back(tmpBuf[1]);
  std::cout << "0x" << std::hex << tmpBuf[0] << " 0x" << tmpBuf[1] << std::dec << std::endl;

  gem::AMC13Event amc13Event;
  amc13Event.setCDFHeader(tmpBuf[0]);
  amc13Event.setAMC13Header(tmpBuf[1]);

  if (uint8_t(tmpBufSize) < amc13Event.nAMC()) {
    std::cout << "code correction is needed: nAMC=" << amc13Event.get_nAMC() << std::endl;
    return false;
  }

  std::cout << "GEMLocalModeDataSource: cb5=" << amc13Event.get_cb5() << std::endl;
  std::cout << "GEMLocalModeDataSource: cb0=" << amc13Event.get_cb0() << std::endl;

  // read AMC headers
  std::cout << "GEMLocalModeDataSource: nAMC=" << amc13Event.get_nAMC() << std::endl;
  n = inpFile.read((char*)tmpBuf, amc13Event.nAMC()*sizeof(uint64_t));
  if ((uint32_t)(n)!=amc13Event.get_nAMC()*8) {
    std::cout << " ERROR got " << n << " chk=" << amc13Event.get_nAMC()*8 << std::endl;
  }
  for (uint8_t ii=0; ii<amc13Event.nAMC(); ii++) {
    amc13Event.addAMCheader(tmpBuf[ii]);
    buf.push_back(tmpBuf[ii]);
    std::cout << "ii=" << ii << " cb4=" << amc13Event.get_cb4(ii) << " amcNr=" << amc13Event.get_amcNr(ii) << " dataSize=" << amc13Event.get_dataSize(ii) << std::endl;
  }

  // read AMC payloads
  for (uint8_t iamc = 0; iamc<amc13Event.nAMC(); ++iamc) {
    gem::AMCdata amcData;
    n = inpFile.read((char*)tmpBuf, 3*sizeof(uint64_t));
    for (int ii=0; ii<3; ii++) buf.push_back(tmpBuf[ii]);
    amcData.setAMCheader1(tmpBuf[0]);
    amcData.setAMCheader2(tmpBuf[1]);
    amcData.setGEMeventHeader(tmpBuf[2]);
    std::cout << "iamc=" << iamc << " amcData.amcNr=" << (uint32_t)(amcData.amcNum()) << " amcData.davCnt=" << (uint32_t)(amcData.davCnt()) << std::endl;

    // read GEB
    for (uint8_t igeb=0; igeb<amcData.davCnt(); igeb++) {
      gem::GEBdata gebData;
      n = inpFile.read((char*)tmpBuf, sizeof(uint64_t));
      buf.push_back(tmpBuf[0]);
      gebData.setChamberHeader(tmpBuf[0]);

      std::cout << "igeb=" << igeb << " " << (uint32_t)(gebData.vfatWordCnt()) << std::endl;
      if (tmpBufSize<gebData.vfatWordCnt()) {
	std::cout << "update code: tmpBufSize=" << tmpBufSize << ", gebData.vfatWordCnt=" << gebData.vfatWordCnt() << std::endl;
	return false;
      }

      n = inpFile.read((char*)tmpBuf, gebData.vfatWordCnt() * sizeof(uint64_t));
      for (int ii=0; ii < gebData.vfatWordCnt(); ii++) {
	buf.push_back(tmpBuf[ii]);
      }

      // read gebData trailer
      n = inpFile.read((char*)tmpBuf, sizeof(uint64_t));
      buf.push_back(tmpBuf[0]);

      // check
      gebData.setChamberTrailer(tmpBuf[0]);
      if (gebData.vfatWordCntT() != gebData.vfatWordCnt()) {
	std::cout << "corrupt data? gebData vfatWordCnt does not match" << std::endl;
      }

    } // end of geb loop

    // read GEMeventTrailer and AMCTrailer
    n = inpFile.read((char*)tmpBuf, 2*sizeof(uint64_t));
    buf.push_back(tmpBuf[0]);
    buf.push_back(tmpBuf[1]);
  } // end of amc loop

  // read AMC13trailer and CDFTrailer
  n = inpFile.read((char*)tmpBuf, 2*sizeof(uint64_t));
  buf.push_back(tmpBuf[0]);
  buf.push_back(tmpBuf[1]);
  // end of amc13Event

  std::cout << "got " << buf.size() << " words\n";

  //
  // create FEDRawData

  auto rawData = std::make_unique<FEDRawData>(sizeof(uint64_t)*buf.size());
  unsigned char *dataptr = rawData->data();

  for (uint16_t i=0; i<buf.size(); i++) {
    ((uint64_t*)dataptr)[i] = buf[i];
  }

  FEDRawData & fedRawData = buffers->FEDData( m_fedid );
  fedRawData = *rawData;

  // get real event number
  uint32_t realeventno = synchronizeEvents();
  int set_runnumber= (m_runnumber!=0) ? m_runnumber : id.run();
  id = edm::EventID(set_runnumber, id.luminosityBlock(), realeventno);
  return true;
}


void GEMLocalModeDataSource::produce(edm::Event &event) {
  std::cout << "GEMLocalModeDataSource::produce" << std::endl;
  event.put(std::move(buffers), "gemLocalModeDataSource");
  buffers.reset();
}

uint32_t GEMLocalModeDataSource::synchronizeEvents() {
  std::cout << "GEMLocalModeDataSource::synchronizeEvents" << std::endl;
  int32_t result= m_currenteventnumber -1;
  return(uint32_t) result;
}


#include "FWCore/Framework/interface/InputSourceMacros.h"
DEFINE_FWK_INPUT_SOURCE(GEMLocalModeDataSource);
