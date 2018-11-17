#ifndef EventFilter_GEMRawToDigi_GEMLocalModeDataSource_h
#define EventFilter_GEMRawToDigi_GEMLocalModeDataSource_h

/** \class GEMLocalModeDataSource
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

#include <iostream>

class GEMLocalModeDataSource : public edm::ProducerSourceFromFiles {
public:
  explicit GEMLocalModeDataSource(const edm::ParameterSet &pset, const edm::InputSourceDescription &desc);
  ~GEMLocalModeDataSource() override;
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);

private:
  bool setRunAndEventInfo(edm::EventID &id, edm::TimeValue_t &time, edm::EventAuxiliary::ExperimentType & et) override;
  void produce(edm::Event &e) override;
  uint32_t synchronizeEvents();

  //void openFile(const std::string &filename);
  unsigned int fileCount() const { return this->fileNames().size(); }

  bool m_hasFerolHeader;
  int m_fedid;   // which FEDId to assign
  uint32_t m_fileindex;
  std::unique_ptr<Storage> storage;
  int m_runnumber;
  uint64_t m_data;
  uint32_t m_currenteventnumber;
  uint32_t m_currenttriggernumber;
  uint32_t m_globaleventnumber;
  int32_t m_eventnumber_shift;
  std::unique_ptr<FEDRawDataCollection> buffers;
};


#endif
