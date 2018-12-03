#ifndef EventFilter_GEMRawToDigi_GEMLocalModeDataSourceDebug_h
#define EventFilter_GEMRawToDigi_GEMLocalModeDataSourceDebug_h

/** \class GEMLocalModeDataSourceDebug
 *  Description: Read GEM data recorded in "local mode" and put into FEDRawCollection
 *  Followed PixelSLinkDataInputSource
 *  \author Andrius Juodagalvis, Vilnius University, Nov 2018
 */

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Sources/interface/ProducerSourceFromFiles.h"
#include "Utilities/StorageFactory/interface/StorageFactory.h"
//#include "Utilities/StorageFactory/interface/StorageAccount.h"
#include "Utilities/StorageFactory/interface/Storage.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

#include <TH1F.h>
#include <TH2F.h>
#include "EventFilter/GEMRawToDigi/interface/VFATdata.h"

#include <iostream>

class GEMLocalModeDataSourceDebug : public edm::ProducerSourceFromFiles {
public:
  explicit GEMLocalModeDataSourceDebug(const edm::ParameterSet &pset, const edm::InputSourceDescription &desc);
  ~GEMLocalModeDataSourceDebug() override;
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);

private:
  bool setRunAndEventInfo(edm::EventID &id, edm::TimeValue_t &time, edm::EventAuxiliary::ExperimentType & et) override;
  void produce(edm::Event &e) override;
  uint32_t synchronizeEvents();

  void debug_studyFile();


  bool m_hasFerolHeader;
  int m_fedid;   // which FEDId to assign
  std::vector<std::string> m_filenames;
  uint32_t m_fileindex; // index of currently opened file
  std::unique_ptr<Storage> storage; // pointer to opened file
  int m_runnumber;
  uint64_t m_data; // local buffer
  uint32_t m_currenteventnumber;
  std::vector<unsigned int> m_processEvents;
  std::unique_ptr<FEDRawDataCollection> buffers;
  uint64_t m_nProcessedEvents, m_nGoodEvents;
  std::vector<uint32_t> m_goodEvents;

  std::vector<gem::VFATdata> m_oldVFATdataV;

  uint32_t m_lastEC;
  TH1F *h1vfatFired;
  TH1F *h1channelFired;
  TH2F *h2vfatChannelFired;
};


#endif
