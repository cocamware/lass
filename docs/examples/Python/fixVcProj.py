# fixVcProj.py
# Author : Tom De Muer (tom_demuer@cocamware.com)
# Copyright : Cocamware 2008-2009.
# All rights reserved.

import re
import os
import string
import xml.dom.minidom

def sanitizeSlashes(iString):
	return iString.replace('\\','/')

def sanitizeQuotes(iString):
	newString = iString[:]
	if newString[0]=='"':
		newString = newString[1:]
	if newString[-1]=='"':
		newString = newString[:-1]
	return newString

def findTagWithAttributeValue(iElement,iTag,iAttribute,iValue):
	firstSelection = iElement.getElementsByTagName(iTag)
	finalSelection = []
	for s in firstSelection:
		if s.hasAttribute(iAttribute):
			if s.getAttribute(iAttribute)==iValue:
				finalSelection.append(s)
	return finalSelection

def fixVcProj(iFilename):
	print "Loading ", iFilename
	content = open(iFilename,"r").read()
	dom = xml.dom.minidom.parseString(content)
	configuration = dom.getElementsByTagName("Configuration")
	for config in configuration:
		configName=config.getAttribute("Name")
		config.setAttribute("OutputDirectory","$(ProjectDir)$(ConfigurationName)")
		config.setAttribute("IntermediateDirectory",r"$(ProjectDir)Intermediate\$(ConfigurationName)")
		tools = config.getElementsByTagName("Tool")

		toolVcLinkerTool = [tool for tool in tools if tool.getAttribute("Name")=="VCLinkerTool"][0]
		toolVcLinkerTool.setAttribute("OutputFile",r"$(OutDir)\embedding%s.pyd" % ["","_d"]["debug" in configName.lower()])
		toolVcLinkerTool.setAttribute("AdditionalLibraryDirectories",r"$(PYTHON_PATH)libs;$(LASS_PATH)lib")
		toolVcLinkerTool.setAttribute("AdditionalDependencies","lass%s.lib" % ["","_d"]["debug" in configName.lower()])

		toolVcCompilerTool = [tool for tool in tools if tool.getAttribute("Name")=="VCCLCompilerTool"][0]
		toolVcCompilerTool.setAttribute("AdditionalIncludeDirectories",r"$(PYTHON_PATH)include;$(LASS_PATH)include")
	open(iFilename,"w").write(dom.toprettyxml())

def fixUser(iFilename):
	print "Loading ", iFilename
	content = open(iFilename,"r").read()
	dom = xml.dom.minidom.parseString(content)
	configuration = dom.getElementsByTagName("Configuration")
	for config in configuration:
		configName=config.getAttribute("Name")

		debugSettings = config.getElementsByTagName("DebugSettings")[0]
		debugSettings.setAttribute("Environment",r"PATH=%LASS_PATH%\bin;%PATH%;")
		debugSettings.setAttribute("WorkingDirectory",r"$(OutDir)")
		debugSettings.setAttribute("Command","$(PYTHON_PATH)python%s.exe" % ["","_d"]["debug" in configName.lower()])
	open(iFilename,"w").write(dom.toprettyxml())

	
for root, dirs, files in os.walk("."):
	for name in [file for file in files if file.endswith(".vcproj")]:
		fixVcProj(os.path.join(root,name))


for root, dirs, files in os.walk("."):
	for name in [file for file in files if file.endswith(".user")]:
		fixUser(os.path.join(root,name))
