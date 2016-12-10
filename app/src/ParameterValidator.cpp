/**
 * \file ParameterValidator.cpp
 *
 * \brief Implementation of validation of XML file and output path
 *
 * \author Kalycito Infotech Private Limited
 *
 * \version 0.1
 *
 */

#include "ParameterValidator.h"

ParameterValidator::ParameterValidator()
{
// REVIEW_COMMENT:
}

ParameterValidator::~ParameterValidator()
{
// REVIEW_COMMENT:
}

ParameterValidator& ParameterValidator::GetInstance()
{
	static ParameterValidator instance;// REVIEW_COMMENT:
	return instance;
}

CliResult ParameterValidator::IsXMLFileValid(std::string xmlFileName)
{
	try
	{
		if (!boost::filesystem::exists(xmlFileName))
		{
			boost::format formatter(kMsgXMLFileNotExists[CliLogger::GetInstance().languageIndex]);
			formatter
			% xmlFileName.c_str();

			return CliResult(CliErrorCode::XML_FILE_NOT_EXISTS, formatter.str());
		}

		if (boost::filesystem::extension(xmlFileName) != ".xml")
		{
			return CliResult(CliErrorCode::XML_FILE_EXTN_INCORRECT, 
								kMsgXMLFileExtnIncorrect[CliLogger::GetInstance().languageIndex]);
		}
	}
	catch(std::exception ex)
	{
		return CliLogger::GetInstance().HandleExceptionCaught("Is XML File Valid", ex);
	}

	return CliResult();
}

CliResult ParameterValidator::IsPathValid(std::string path)
{
	try
	{
		if (!boost::filesystem::exists(path))
		{
			boost::format formatter(kMsgOutputPathNotExiists[CliLogger::GetInstance().languageIndex]);
			formatter
			% path.c_str();

			return CliResult(CliErrorCode::OUTPUT_PATH_NOT_EXISTS, formatter.str());
		}
	}
	catch(std::exception ex)
	{
		return CliLogger::GetInstance().HandleExceptionCaught("Is Path Valid", ex);
	}

	return CliResult();
}

CliResult ParameterValidator::IsXMLSchemaValid(std::string xmlFileName)
{
	return IsSchemaValid(xmlFileName, kXmlSchemaDefinitionFileName);
}

CliResult ParameterValidator::IsXDCSchemaValid(std::string xdcFileName)
{
	return IsSchemaValid(xdcFileName, kXdcSchemaDefinitionFileName);
}

CliResult ParameterValidator::IsSchemaValid(std::string fileName, const std::string schemaDefFile)
{
	try
	{
		/** Initialize the Xerces usage */
		xercesc::XMLPlatformUtils::Initialize();
// REVIEW_COMMENT:
		xercesc::XercesDOMParser* domParserXdc = new xercesc::XercesDOMParser();

		/** Validate for the schema file existance */
		if (!boost::filesystem::exists(schemaDefFile))
		{
			boost::format formatter(kMsgSchemaFileNotExists[CliLogger::GetInstance().languageIndex]);
			formatter
			% schemaDefFile.c_str();

			return CliResult(CliErrorCode::SCHEMA_FILE_NOT_EXISTS, formatter.str());
		}

		/** Input project file to DOM parse() function */
		domParserXdc->parse(fileName.c_str()); 

		if (domParserXdc->loadGrammar(schemaDefFile.c_str(), 
									xercesc::Grammar::SchemaGrammarType) == NULL)
		{
			return CliResult(CliErrorCode::ERROR_LOADING_GRAMMER, 
								kMsgErrorLoadingGrammer[CliLogger::GetInstance().languageIndex]);
		}

		/** Set validation checks required for the file */
		domParserXdc->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
		domParserXdc->setDoNamespaces(true);
		domParserXdc->setDoXInclude(true);
		domParserXdc->setDoSchema(true);
		domParserXdc->setValidationConstraintFatal(true);

		if (domParserXdc->getErrorCount() != 0)
		{
			CliLogger::GetInstance().LogMessage(CliMessageType::CLI_INFO, 
												"DOM Parser error count" + domParserXdc->getErrorCount());

			boost::format formatter(kMsgFileSchemeNotValid[CliLogger::GetInstance().languageIndex]);
			formatter
			% fileName.c_str();

			return CliResult(CliErrorCode::FILE_SCHEMA_NOT_VALID, formatter.str());
		}

		delete [] domParserXdc;

		/** Release the Xerces usage */
		xercesc::XMLPlatformUtils::Terminate();
	}
	catch(std::exception ex)
	{
		return CliLogger::GetInstance().HandleExceptionCaught("Is Schema Valid", ex);
	}

	return CliResult();
}
