/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/opengl/shader/shadermanager.h>
#include <modules/abuffergl/abufferglmodule.h>
#include <modules/abuffergl/processors/abuffergeometryglprocessor.h>

// Autogenerated
#include <modules/abuffergl/shader_resources.h>

namespace inviwo {

class CompositePropertyWidgetQt;

ABufferGLModule::ABufferGLModule(InviwoApplication* app) : InviwoModule(app, "ABufferGL") {

    abuffergl::addShaderResources(
        ShaderManager::getPtr(),
        {getPath(ModulePath::GLSL), getPath(ModulePath::GLSL) + "/abuffer"});

    registerProcessor<ABufferGeometryGLProcessor>();
}

int ABufferGLModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> ABufferGLModule::getConverter(int version) const {
    return util::make_unique<Converter>(version);
}

ABufferGLModule::Converter::Converter(int version) : version_(version) {}

bool ABufferGLModule::Converter::convert(TxElement* root) {
    const std::vector<xml::IdentifierReplacement> repl = {
        // ABufferGeometryGLProcessor
        {{xml::Kind::processor("org.inviwo.ABufferGeometryGLProcessor"),
          xml::Kind::inport("org.inviwo.MeshFlatMultiInport")},
         "geometry.inport",
         "geometry"},
        {{xml::Kind::processor("org.inviwo.ABufferGeometryGLProcessor"),
          xml::Kind::outport("org.inviwo.ImageOutport")},
         "image.outport",
         "image"}};

    bool res = false;
    switch (version_) {
        case 0: {
            res |= xml::changeIdentifiers(root, repl);
        }
            return res;

        default:
            return false;  // No changes
    }
    return true;
}

}  // namespace inviwo
