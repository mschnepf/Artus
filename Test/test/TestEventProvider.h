/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/PipelineRunner.h"
#include "Artus/Core/interface/EventProviderBase.h"

class TestEventProvider: public EventProviderBase<TestTypes> {
public:
	~TestEventProvider() {}

	virtual TestTypes::event_type const& GetCurrentEvent() const override {
		return m_event;
	}
	virtual bool GetEntry(long long lEventNumber) override {
		return lEventNumber < GetEntries();
	}
	virtual long long GetEntries() const override {
		return 10;
	}

	TestTypes::event_type m_event;
};
