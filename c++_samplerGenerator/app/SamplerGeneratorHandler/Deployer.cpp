//
// Created by frivas on 27/03/17.
//

#include <SampleGeneratorLib/DatasetConverters/liveReaders/GenericLiveReader.h>
#include <gui/Utils.h>
#include <SampleGeneratorLib/FrameworkEvaluator/GenericInferencer.h>
#include <SampleGeneratorLib/FrameworkEvaluator/MassInferencer.h>
#include "Deployer.h"
#include "SamplerGenerationHandler.h"

void
SampleGeneratorHandler::Deployer::process(QListView *deployImpList, QListView *weightsList, QListView *netConfigList,
                                          QListView *inferencerImpList, QListView *inferencerNamesList,
                                          const std::string &weightsPath, const std::string &cfgPath,
                                          const std::string &inferencerNamesPath, const std::string &inputInfo) {

    GenericLiveReaderPtr reader = SamplerGenerationHandler::createLiveReaderPtr( inferencerNamesList,
                                                                                 deployImpList,inputInfo,inferencerNamesPath);

    std::vector<std::string> weights;
    if (! Utils::getListViewContent(weightsList,weights,weightsPath+ "/")){
        LOG(WARNING)<<"Select the weightsList";
        return;
    }

    std::vector<std::string> netConfiguration;
    if (! Utils::getListViewContent(netConfigList,netConfiguration,cfgPath+ "/")){
        LOG(WARNING)<<"Select the netConfiguration";
        return;
    }

    std::vector<std::string> inferencerImp;
    if (! Utils::getListViewContent(inferencerImpList,inferencerImp,"")){
        LOG(WARNING)<<"Select the inferencer type";
        return;
    }

    std::vector<std::string> inferencerNames;
    if (! Utils::getListViewContent(inferencerNamesList,inferencerNames,inferencerNamesPath + "/")){
        LOG(WARNING)<<"Select the inferencer type";
        return;
    }

    GenericInferencerPtr inferencer(new GenericInferencer(netConfiguration[0],weights[0],inferencerNames[0],inferencerImp[0]));
    MassInferencer massInferencer(reader->getReader(),inferencer->getInferencer(),"./tmp", true);
    massInferencer.process(false);
}
