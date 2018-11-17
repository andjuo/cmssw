#include "EventFilter/GEMRawToDigi/plugins/GEMLocalModeDataSource.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

GEMLocalModeDataSource::GEMLocalModeDataSource(const edm::ParameterSet & pset, edm::InputSourceDescription const &desc) :
  edm::ProducerSourceFromFiles(pset,desc,true), // true - RealData
  m_hasFerolHeader( pset.getUntrackedParameter<bool>("hasFerolHeader",false)),
  m_fedid( pset.getUntrackedParameter<int>("fedid")),
  m_fileindex(0),
  m_runnumber( pset.getUntrackedParameter<int>("runNumber",-1)),
  m_currenteventnumber(0),
  m_currenttriggernumber(0),
  m_eventnumber_shift(0)
{
  produces<FEDRawDataCollection>();

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
  //desc.add<edm::InputTag>("InputLabel", edm::InputTag("rawDataCollector")); 
  //desc.add<bool>("useDBEMap", false);
  //desc.add<bool>("unPackStatusDigis", false);
  descriptions.addDefault(desc);  
}


bool GEMLocalModeDataSource::setRunAndEventInfo(edm::EventID &id, edm::TimeValue_t &time, edm::EventAuxiliary::ExperimentType &)
{
  Storage & inpFile = *storage;

  // create product (raw data)
  buffers.reset( new FEDRawDataCollection );

  // no loop over data, assuming record is 1 event

  std::vector<uint64_t> buf;
  inpFile.read((char*)&m_data,8); // number of bytes to read
  buf.push_back(m_data);

  /*
  auto outGEMDigis = std::make_unique<GEMDigiCollection>();
  auto outVFATStatus = std::make_unique<GEMVfatStatusDigiCollection>();
  auto outGEBStatus = std::make_unique<GEMGEBdataCollection>();
  auto outAMCdata = std::make_unique<GEMAMCdataCollection>();
  auto outAMC13Event = std::make_unique<GEMAMC13EventCollection>();

  // Take raw from the event
  edm::Handle<FEDRawDataCollection> fed_buffers;
  iEvent.getByToken( fed_token, fed_buffers );
  
  auto gemROMap = runCache(iEvent.getRun().index());
  
  for (unsigned int id=FEDNumbering::MINGEMFEDID; id<=FEDNumbering::MAXGEMFEDID; ++id) { 
    const FEDRawData& fedData = fed_buffers->FEDData(id);
    
    int nWords = fedData.size()/sizeof(uint64_t);
    LogDebug("GEMLocalModeDataSource") <<" words " << nWords;
    
    if (nWords<5) continue;
    const unsigned char * data = fedData.data();
    
    auto amc13Event = std::make_unique<AMC13Event>();
    
    const uint64_t* word = reinterpret_cast<const uint64_t* >(data);
    
    amc13Event->setCDFHeader(*word);
    amc13Event->setAMC13Header(*(++word));
    
    // Readout out AMC headers
    for (uint8_t i = 0; i < amc13Event->nAMC(); ++i)
      amc13Event->addAMCheader(*(++word));
    
    // Readout out AMC payloads
    for (uint8_t i = 0; i < amc13Event->nAMC(); ++i) {
      auto amcData = std::make_unique<AMCdata>();
      amcData->setAMCheader1(*(++word));      
      amcData->setAMCheader2(*(++word));
      amcData->setGEMeventHeader(*(++word));
      uint16_t amcId = amcData->boardId();
      uint16_t amcBx = amcData->bx();

      // Fill GEB
      for (uint8_t j = 0; j < amcData->davCnt(); ++j) {
	auto gebData = std::make_unique<GEBdata>();
	gebData->setChamberHeader(*(++word));
	
	uint16_t gebId = gebData->inputID();
	uint16_t vfatId=0;
	GEMROmap::eCoord geb_ec = {amcId, gebId, vfatId};
	GEMROmap::dCoord geb_dc = gemROMap->hitPosition(geb_ec);
	GEMDetId gemId = geb_dc.gemDetId;

	for (uint16_t k = 0; k < gebData->vfatWordCnt()/3; k++) {
	  auto vfatData = std::make_unique<VFATdata>();
	  vfatData->read_fw(*(++word));
	  vfatData->read_sw(*(++word));
	  vfatData->read_tw(*(++word));
	  
	  if (geb_dc.vfatType < 10) {
	    // vfat v2
	    vfatId = vfatData->chipID();
	    vfatData->setVersion(2);
	  }
	  else {
	    // vfat v3
	    vfatId = vfatData->position();
	    vfatData->setVersion(3);
	  }
	  
	  uint16_t bc=vfatData->bc();
	  // strip bx = vfat bx - amc bx
	  int bx = bc-amcBx;
	  
	  if (vfatData->quality()) {
	    edm::LogWarning("GEMLocalModeDataSource") << "Quality "<< vfatData->quality()
						  << " b1010 "<< int(vfatData->b1010())
						  << " b1100 "<< int(vfatData->b1100())
						  << " b1110 "<< int(vfatData->b1110());
	    if (vfatData->crc() != vfatData->checkCRC() ) {
	      edm::LogWarning("GEMLocalModeDataSource") << "DIFFERENT CRC :"
						    <<vfatData->crc()<<"   "<<vfatData->checkCRC();	      
	    }
	  }
	  
	  //check if ChipID exists.
	  GEMROmap::eCoord ec = {amcId, gebId, vfatId};
	  if (!gemROMap->isValidChipID(ec)) {
	    edm::LogWarning("GEMLocalModeDataSource") << "InValid: amcId "<<ec.amcId
						  << " gebId "<< ec.gebId
						  << " vfatId "<< ec.vfatId;
	    continue;
	  }

	  GEMROmap::dCoord dc = gemROMap->hitPosition(ec);
	  gemId = dc.gemDetId;
	  vfatData->setPhi(dc.locPhi);

	  for (int chan = 0; chan < VFATdata::nChannels; ++chan) {
	    uint8_t chan0xf = 0;
	    if (chan < 64) chan0xf = ((vfatData->lsData() >> chan) & 0x1);
	    else chan0xf = ((vfatData->msData() >> (chan-64)) & 0x1);

	    // no hits
	    if (chan0xf==0) continue;
	    	             
            GEMROmap::channelNum chMap = {dc.vfatType, chan};
            GEMROmap::stripNum stMap = gemROMap->hitPosition(chMap);

            int stripId = stMap.stNum + vfatData->phi()*GEMELMap::maxChan_;    

	    GEMDigi digi(stripId,bx);
	    LogDebug("GEMLocalModeDataSource") <<" vfatId "<<ec.vfatId
					   <<" gemDetId "<< gemId
					   <<" chan "<< chMap.chNum
					   <<" strip "<< stripId
					   <<" bx "<< digi.bx();
	    
	    outGEMDigis.get()->insertDigi(gemId,digi);
	    
	  }// end of channel loop
	  
	  if (unPackStatusDigis_) {
            outVFATStatus.get()->insertDigi(gemId, GEMVfatStatusDigi(*vfatData));
	  }
	  
	} // end of vfat loop
	
	gebData->setChamberTrailer(*(++word));
	
        if (unPackStatusDigis_) {
	  outGEBStatus.get()->insertDigi(gemId.chamberId(), (*gebData)); 
        }
	
      } // end of geb loop
      
      amcData->setGEMeventTrailer(*(++word));
      amcData->setAMCTrailer(*(++word));
      
      if (unPackStatusDigis_) {
        outAMCdata.get()->insertDigi(amcData->boardId(), (*amcData));
      }

    } // end of amc loop
    
    amc13Event->setAMC13Trailer(*(++word));
    amc13Event->setCDFTrailer(*(++word));
     
    if (unPackStatusDigis_) {
      outAMC13Event.get()->insertDigi(amc13Event->bxId(), AMC13Event(*amc13Event));
    }
    
  } // end of amc13Event
  */

  std::cout << "got " << buf.size() << " words\n";

  auto rawData = std::make_unique<FEDRawData>(8*buf.size());
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
  event.put(std::move(buffers));
  buffers.reset();
}

uint32_t GEMLocalModeDataSource::synchronizeEvents() {
  int32_t result= m_currenteventnumber -1;
  return(uint32_t) result;
}


#include "FWCore/Framework/interface/InputSourceMacros.h"
DEFINE_FWK_INPUT_SOURCE(GEMLocalModeDataSource);
