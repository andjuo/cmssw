#include "EventFilter/GEMRawToDigi/plugins/GEMLocalModeDataSource.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "EventFilter/GEMRawToDigi/interface/AMC13Event.h"
#include "EventFilter/GEMRawToDigi/interface/VFATdata.h"


GEMLocalModeDataSource::GEMLocalModeDataSource(const edm::ParameterSet & pset, edm::InputSourceDescription const &desc) :
  edm::ProducerSourceFromFiles(pset,desc,true), // true - RealData
  m_hasFerolHeader( pset.getUntrackedParameter<bool>("hasFerolHeader",false)),
  m_fedid( pset.getUntrackedParameter<int>("fedId", 12345)),
  m_fileindex(0),
  m_runnumber( pset.getUntrackedParameter<int>("runNumber",-1)),
  m_currenteventnumber(0),
  m_currenttriggernumber(0),
  m_eventnumber_shift(0)
{
  produces<FEDRawDataCollection>("gemLocalModeDataSource");

  if (m_fileindex >= this->fileNames().size()) {
    edm::LogInfo("") << "no more file to read" << std::endl;
    return;
  }
  std::string currentfilename = this->fileNames()[m_fileindex];
  std::cout << "examining " << currentfilename << std::endl;
  m_fileindex++;

  IOOffset size = -1;
  StorageFactory::getToModify()->enableAccounting(true);
  bool exists = StorageFactory::get()->check(currentfilename, &size);
  std::cout << "file size " << size << std::endl;
  if (!exists) {
    edm::LogInfo("") << "file " << currentfilename << " cannot be found" << std::endl;
    return;
  }

  // open file stream
  storage = StorageFactory::get()->open(currentfilename);
  // (throw if storage is 0)
  if (!storage) {
    std::cout << "failed to open the file" << std::endl;
  }

  m_runnumber = m_fileindex; // dummy run number
  m_currenteventnumber = 1; // reset counter
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
  std::cout << "setRunAndEventInfo" << std::endl;

  Storage & inpFile = *storage;

  // create product (raw data)
  buffers.reset( new FEDRawDataCollection );

  // no loop over data, assuming record is 1 event
  m_currenteventnumber++;

  std::cout << "the number= " << sizeof(uint64_t) << std::endl;
  std::vector<uint64_t> buf;
  const int tmpBufSize=24;
  uint64_t tmpBuf[tmpBufSize];

  // get CDFHeader and AMC13Header
  int n=inpFile.read((char*)tmpBuf,3*sizeof(uint64_t)); // number of bytes to read
  if (n!=3*sizeof(uint64_t)) {
    std::cout << "end of input file" << std::endl;
    return false;
  }
  std::cout << "got 1st data " << tmpBuf[0] << std::endl;
  buf.push_back(tmpBuf[0]);
  buf.push_back(tmpBuf[1]);
  buf.push_back(tmpBuf[2]);

  gem::AMC13Event amc13Event;
  // tmpBuf[1] is not used
  amc13Event.setCDFHeader(tmpBuf[0]);
  amc13Event.setAMC13Header(tmpBuf[2]);

  if (uint8_t(tmpBufSize) < amc13Event.nAMC()) {
    std::cout << "code correction is needed: nAMC=" << amc13Event.nAMC() << std::endl;
    return false;
  }

  // read AMC headers
  std::cout << "GEMLocalModeDataSource: nAMC=" << amc13Event.nAMC() << std::endl;
  n = inpFile.read((char*)tmpBuf, amc13Event.nAMC()*sizeof(uint64_t));
  for (uint8_t ii=0; ii<amc13Event.nAMC(); ii++) {
    amc13Event.addAMCheader(tmpBuf[ii]);
    buf.push_back(tmpBuf[ii]);
  }

  // read AMC payloads
  for (uint8_t iamc = 0; iamc<amc13Event.nAMC(); ++iamc) {
    gem::AMCdata amcData;
    n = inpFile.read((char*)tmpBuf, 3*sizeof(uint64_t));
    for (int ii=0; ii<3; ii++) buf.push_back(tmpBuf[ii]);
    amcData.setAMCheader1(tmpBuf[0]);
    amcData.setAMCheader2(tmpBuf[1]);
    amcData.setGEMeventHeader(tmpBuf[2]);

    // read GEB
    for (uint8_t igeb=0; igeb<amcData.davCnt(); igeb++) {
      gem::GEBdata gebData;
      n = inpFile.read((char*)tmpBuf, sizeof(uint64_t));
      buf.push_back(tmpBuf[0]);
      gebData.setChamberHeader(tmpBuf[0]);

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
