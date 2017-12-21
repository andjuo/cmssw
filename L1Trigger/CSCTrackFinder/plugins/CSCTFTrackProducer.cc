#include "L1Trigger/CSCTrackFinder/plugins/CSCTFTrackProducer.h"

#include "L1Trigger/CSCTrackFinder/src/CSCTFTrackBuilder.h"

#include <vector>
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCTrackCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/CSCTriggerContainer.h"
#include "DataFormats/L1CSCTrackFinder/interface/TrackStub.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/L1TObjects/interface/L1MuTriggerScales.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerScalesRcd.h"
#include "CondFormats/L1TObjects/interface/L1MuTriggerPtScale.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerPtScaleRcd.h"

CSCTFTrackProducer::CSCTFTrackProducer(const edm::ParameterSet& pset)
{
  input_module = consumes<CSCCorrelatedLCTDigiCollection>(pset.getUntrackedParameter<edm::InputTag>("SectorReceiverInput"));
  dt_producer  = consumes<L1MuDTChambPhContainer>(pset.getUntrackedParameter<edm::InputTag>("DTproducer"));
  directProd   = consumes<CSCTriggerContainer<csctf::TrackStub> >(pset.getUntrackedParameter<edm::InputTag>("DtDirectProd"));
  sp_pset = pset.getParameter<edm::ParameterSet>("SectorProcessor");
  useDT = pset.getParameter<bool>("useDT");
  readDtDirect = pset.getParameter<bool>("readDtDirect");
  TMB07 = pset.getParameter<bool>("isTMB07");
  my_dtrc = new CSCTFDTReceiver();
  m_scalesCacheID = 0ULL ;
  m_ptScaleCacheID = 0ULL ;
  my_builder = nullptr ;
  produces<L1CSCTrackCollection>();
  produces<CSCTriggerContainer<csctf::TrackStub> >();
}

CSCTFTrackProducer::~CSCTFTrackProducer()
{
  delete my_dtrc;
  my_dtrc = nullptr;

  delete my_builder;
  my_builder = nullptr;
}

void CSCTFTrackProducer::beginJob(){
  //  my_builder->initialize(es);
}

void CSCTFTrackProducer::produce(edm::Event & e, const edm::EventSetup& c)
{
  // Update CSCTFTrackBuilder only if the scales have changed.  Use the
  // EventSetup cacheIdentifier to tell when this has happened.
  if(  c.get< L1MuTriggerScalesRcd >().cacheIdentifier() != m_scalesCacheID ||
       c.get< L1MuTriggerPtScaleRcd >().cacheIdentifier() != m_ptScaleCacheID )
    {
      if(my_builder) delete my_builder ;

      edm::ESHandle< L1MuTriggerScales > scales ;
      c.get< L1MuTriggerScalesRcd >().get( scales ) ;

      edm::ESHandle< L1MuTriggerPtScale > ptScale ;
      c.get< L1MuTriggerPtScaleRcd >().get( ptScale ) ;

      my_builder = new CSCTFTrackBuilder(sp_pset,TMB07,
					 scales.product(),ptScale.product());
      my_builder->initialize(c);

      m_scalesCacheID = c.get< L1MuTriggerScalesRcd >().cacheIdentifier() ;
      m_ptScaleCacheID = c.get< L1MuTriggerPtScaleRcd >().cacheIdentifier() ;
    }

  // set geometry pointer
  edm::ESHandle<CSCGeometry> pDD;
  c.get<MuonGeometryRecord>().get( pDD );

  edm::Handle<CSCCorrelatedLCTDigiCollection> LCTs;
  std::unique_ptr<L1CSCTrackCollection> track_product(new L1CSCTrackCollection);
  e.getByToken(input_module, LCTs);
  std::unique_ptr<CSCTriggerContainer<csctf::TrackStub> > dt_stubs(new CSCTriggerContainer<csctf::TrackStub>);

  // Either emulate or directly read in DT stubs based on switch
  //////////////////////////////////////////////////////////////
  CSCTriggerContainer<csctf::TrackStub> emulStub;
  if(readDtDirect == false)
  {
    edm::Handle<L1MuDTChambPhContainer> dttrig;
	e.getByToken(dt_producer, dttrig);
	emulStub = my_dtrc->process(dttrig.product());
  } else {
    edm::Handle<CSCTriggerContainer<csctf::TrackStub> > stubsFromDaq;
    //e.getByLabel("csctfunpacker","DT",stubsFromDaq);
	e.getByToken(directProd, stubsFromDaq);
	const CSCTriggerContainer<csctf::TrackStub>* stubPointer = stubsFromDaq.product();
	emulStub.push_many(*stubPointer);
  }

  my_builder->buildTracks(LCTs.product(), (useDT?&emulStub:nullptr), track_product.get(), dt_stubs.get());

  e.put(std::move(track_product));
  e.put(std::move(dt_stubs));
}
