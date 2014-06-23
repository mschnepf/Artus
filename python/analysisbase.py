# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.processor as processor


class AnalysisBase(processor.Processor):
	def __init__(self):
		super(AnalysisBase, self).__init__()
	
	def prepare_args(self, plotData):
		super(AnalysisBase, self).prepare_args(plotData)
	
	def run(self, plotdict=None):
		super(AnalysisBase, self).run(plotdict)

