#include <modules/base/basemodule.h>

#include <modules/base/processors/cubeproxygeometry.h>
#include <modules/base/processors/entryexitpoints.h>
#include <modules/base/processors/findedges.h>
#include <modules/base/processors/firstivwprocessor.h>
#include <modules/base/processors/imagegrayscale.h>
#include <modules/base/processors/imagemixer.h>
#include <modules/base/processors/imagesource.h>
#include <modules/base/processors/minmaxprocessor.h>
#include <modules/base/processors/redgreenprocessor.h>
#include <modules/base/processors/simpleraycaster.h>
#include <modules/base/processors/testprocessor.h>
#include <modules/base/processors/transferfunctiontestprocessor.h>
#include <modules/base/processors/volumesplit.h>
#include <modules/base/processors/volumesplitcompositor.h>
#include <modules/base/processors/volumesplitimagecompositor.h>

namespace inviwo {

BaseModule::BaseModule() : InviwoModule() {
    setIdentifier("Base");
    setXMLFileName("base/basemodule.xml");

    addProcessor(new CubeProxygeometry());
    addProcessor(new EntryExitPoints());
	addProcessor(new FindEdges());
	addProcessor(new FirstIvwProcessor());
	addProcessor(new ImageGrayscale());
    addProcessor(new ImageMixer());
    addProcessor(new ImageSource());
    addProcessor(new MinMaxProcessor());
    addProcessor(new RedGreenProcessor());
    addProcessor(new SimpleRaycaster());
    addProcessor(new TestProcessor());
    addProcessor(new TransferFunctiontionTestProcessor());
    addProcessor(new VolumeSplit());
    addProcessor(new VolumeSplitCompositor());
    addProcessor(new VolumeSplitImageCompositor());
}

} // namespace
