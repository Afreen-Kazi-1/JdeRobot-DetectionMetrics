//
// Created by frivas on 4/02/17.
//

#ifndef SAMPLERGENERATOR_GENERICDATASETREADER_H
#define SAMPLERGENERATOR_GENERICDATASETREADER_H

#include <string>
#include "SpinelloDatasetReader.h"
#include "YoloDatasetReader.h"
#include <DatasetConverters/readers/DatasetReader.h>
#include "OwnDatasetReader.h"

enum READER_IMPLEMENTATIONS{OWN, SPINELLO, YOLO};


class GenericDatasetReader {
public:
    GenericDatasetReader(const std::string& path, const std::string& classNamesFile, const std::string& readerImplementation);
    GenericDatasetReader(const std::vector<std::string>& paths,const std::string& classNamesFile, const std::string& readerImplementation);

    DatasetReaderPtr getReader();

    static std::vector<std::string> getAvailableImplementations();

private:
    READER_IMPLEMENTATIONS imp;
    OwnDatasetReaderPtr ownDatasetReaderPtr;
    YoloDatasetReaderPtr yoloDatasetReaderPtr;
    SpinelloDatasetReaderPtr spinelloDatasetReaderPtr;

    std::vector<std::string> availableImplementations;

    static void configureAvailablesImplementations(std::vector<std::string>& data);
    READER_IMPLEMENTATIONS getImplementation(const std::string& readerImplementation);
};


typedef boost::shared_ptr<GenericDatasetReader> GenericDatasetReaderPtr;

#endif //SAMPLERGENERATOR_GENERICDATASETREADER_H
