
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"

/**
   \brief GlobalProducer, for valid taus.
   
   ... no comment ...
*/

template<class TTypes>
class ValidTausProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() ARTUS_CPP11_OVERRIDE {
		return "valid_taus";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		for (KDataPFTaus::iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); tau++)
		{
			bool validTau = true;
			
			// TODO
			
			if (validTau)
				product.m_validTaus.push_back(&(*tau));
			else
				product.m_invalidTaus.push_back(&(*tau));
		}

		return true;
	}

	// empty to serve as a pure global producer
	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
	}
};

