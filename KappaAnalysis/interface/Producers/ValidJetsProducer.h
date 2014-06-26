
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief Producer for valid jets.
   
   Valid jets pass the particle flow JetID ("tight", "medium" or "loose") by JetMET. There are no
   pileupJetID requirements applied at the moment.

   This producer requires the "JetID" config tag to be set to "tight", "medium" or "loose".
   
   This producer should be run after the ValidElectronsProducer, ValidMuonsProducer and ValidTausProducer,
   because it cleans the list of jets according to the valid leptons.
   
   This is a templated base version. Use the actual versions for KDataPFJets or KDataPFTaggedJets
   at the end of this file.
*/


template<class TTypes, class TJet, class TValidJet>
class ValidJetsProducerBase: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, TValidJet>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class JetID : int
	{
		NONE  = -1,
		TIGHT = 0,
		MEDIUM = 1,
		LOOSE  = 2,
	};
	static JetID ToJetID(std::string const& jetID)
	{
		if (jetID == "tight") return JetID::TIGHT;
		else if (jetID == "medium") return JetID::MEDIUM;
		else if (jetID == "loose") return JetID::LOOSE;
		else return JetID::NONE;
	}
	
	ValidJetsProducerBase(std::vector<TJet>* event_type::*jets, std::vector<TValidJet*> product_type::*validJets) :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, TValidJet>(validJets),
		m_jetsMember(jets)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		jetID = ToJetID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID())));
		
		this->lowerPtCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetJetLowerPtCuts()),
		                                                                 this->lowerPtCutsByHltName);
		this->upperAbsEtaCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetJetUpperAbsEtaCuts()),
		                                                                 this->upperAbsEtaCutsByHltName);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (typename std::vector<TJet>::iterator jet = (event.*m_jetsMember)->begin();
		     jet != (event.*m_jetsMember)->end(); ++jet)
		{
			bool validJet = true;

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// jets, all eta
			float maxNeutralFraction = this->GetMaxNeutralFraction();
			
			validJet = validJet
					   && jet->neutralHadFraction + jet->HFHadFraction < maxNeutralFraction
					   && jet->neutralEMFraction < maxNeutralFraction
					   && jet->nConst > 1;
			// jets, |eta| < 2.4 (tracker)
			if (std::abs(jet->p4.eta()) < 2.4)
			{
				validJet = validJet
						   && jet->chargedHadFraction > 0.0
						   && jet->nCharged > 0
						   && jet->chargedEMFraction < 0.99;
			}
			
			// remove leptons from list of jets via simple DeltaR isolation
			for (std::vector<KLepton*>::const_iterator lepton = product.m_validLeptons.begin();
			     validJet && lepton != product.m_validLeptons.end(); ++lepton)
			{
				validJet = validJet && ROOT::Math::VectorUtil::DeltaR(jet->p4, (*lepton)->p4) > settings.GetJetLeptonLowerDeltaRCut();
			}
			
			// kinematic cuts
			validJet = validJet && this->PassKinematicCuts(&(*jet), event, product);
			
			// check possible analysis-specific criteria
			validJet = validJet && AdditionalCriteria(&(*jet), event, product, settings);

			if (validJet)
				product.m_validJets.push_back(&(*jet));
			else
				product.m_invalidJets.push_back(&(*jet));
		}
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validJet = true;
		
		return validJet;
	}


private:
	std::vector<TJet>* event_type::*m_jetsMember;
	JetID jetID;

	float GetMaxNeutralFraction() const
	{
		float maxNeutralFraction = 0.0;
		
		if (jetID == JetID::TIGHT)
		{
			maxNeutralFraction = 0.90;
		}
		else if (jetID == JetID::MEDIUM)
		{
			maxNeutralFraction = 0.95;
		}
		else if (jetID == JetID::LOOSE)
		{
			maxNeutralFraction = 0.99;
		}
		else if (jetID != JetID::NONE)
		{
			LOG(FATAL) << "Jet ID of type " << Utility::ToUnderlyingValue(jetID) << " not yet implemented!";
		}
		
		return maxNeutralFraction;
	}

};



/**
   \brief Producer for valid jets (simple PF jets).
   
   Operates on the vector event.m_jets.
*/
template<class TTypes>
class ValidJetsProducer: public ValidJetsProducerBase<TTypes, KDataPFJet, KDataPFJet>
{
public:
	ValidJetsProducer() : ValidJetsProducerBase<TTypes, KDataPFJet, KDataPFJet>(&TTypes::event_type::m_jets,
	                                                                            &TTypes::product_type::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_jets";
	}
};



/**
   \brief Producer for valid jets (tagged PF jets).
   
   Operates on the vector event.m_tjets.
   
   Required config tags:
   - PuJetIDs
*/
template<class TTypes>
class ValidTaggedJetsProducer: public ValidJetsProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	ValidTaggedJetsProducer() : ValidJetsProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>(&TTypes::event_type::m_tjets,
	                                                                                        &TTypes::product_type::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_tagged_jets";
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ValidJetsProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>::Init(settings);
		
		puJetIdsByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap(settings.GetPuJetIDs()),
		                                                              puJetIdsByHltName);
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validJet = ValidJetsProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>::AdditionalCriteria(jet, event, product, settings);
		
		// PU Jet ID
		for (std::map<size_t, std::vector<std::string> >::const_iterator puJetIdByIndex = puJetIdsByIndex.begin();
		     puJetIdByIndex != puJetIdsByIndex.end() && validJet; ++puJetIdByIndex)
		{
			if (puJetIdByIndex->first == product.m_validJets.size())
			{
				validJet = validJet && PassPuJetIds(jet, puJetIdByIndex->second, event.m_taggermetadata);
			}
		}
		
		for (std::map<std::string, std::vector<std::string> >::const_iterator puJetIdByHltName = puJetIdsByHltName.begin();
		     puJetIdByHltName != puJetIdsByHltName.end() && validJet; ++puJetIdByHltName)
		{
			if (puJetIdByHltName->first == "default")
			{
				validJet = validJet && PassPuJetIds(jet, puJetIdByHltName->second, event.m_taggermetadata);
			}
			else
			{
				LOG(FATAL) << "HLT name dependent PU Jet is not yet implemented!";
			}
		}
		
		return validJet;
	}

private:
	std::map<size_t, std::vector<std::string> > puJetIdsByIndex;
	std::map<std::string, std::vector<std::string> > puJetIdsByHltName;
	
	bool PassPuJetIds(KDataPFTaggedJet* jet, std::vector<std::string> const& puJetIds, KTaggerMetadata* taggerMetadata) const
	{
		bool validJet = true;
		
		for (std::vector<std::string>::const_iterator puJetId = puJetIds.begin();
		     puJetId != puJetIds.end() && validJet; ++puJetId)
		{
			validJet = validJet && jet->getpuJetID(*puJetId, taggerMetadata);
		}
		
		return validJet;
	}
};


