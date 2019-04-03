/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2018 Inviwo Foundation
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

#include <modules/zmq/processors/zmqvolumeboxpublisher.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/networklock.h>
#include <limits>
#include <Windows.h>

namespace inviwo {

const ProcessorInfo ZmqVolumeBoxProcessor::processorInfo_{
    "org.inviwo.ZmqVolumeBoxProcessor",         // Class identifier
    "ZmqVolumeBoxProcessor",                    // Display name
    "Image Operation",        // Category
    CodeState::Experimental,  // Code state
    Tags::GL,                 // Tags
};
const ProcessorInfo ZmqVolumeBoxProcessor::getProcessorInfo() const { return processorInfo_; }

ZmqVolumeBoxProcessor::ZmqVolumeBoxProcessor()
    : Processor()
    , volume_("volume") 
	, context(zmq::context_t(1))
	, box_socket(zmq::socket_t(context, ZMQ_PUB)) {

    addPort(volume_);

	box_socket.bind("tcp://*:12346");
}

ZmqVolumeBoxProcessor::~ZmqVolumeBoxProcessor() {}

void ZmqVolumeBoxProcessor::process() { 
	sendZMQ();
}

void ZmqVolumeBoxProcessor::sendZMQ() { 
    mat3 basis = volume_.getData()->getBasis();
    vec3 offset = volume_.getData()->getOffset();
    vec3 left_lower = vec3(0, 0, 0);
    vec3 right_upper = vec3(1, 1, 1);
    left_lower = left_lower * basis + offset;
    right_upper = right_upper * basis + offset;
    json json_message = {
		{"leftLower", {left_lower.x, left_lower.y, left_lower.z}},
		{"rightUpper", {right_upper.x, right_upper.y, right_upper.z}}
	};
	
	std::string string_message = json_message.dump();
    zmq::message_t message = zmq::message_t(string_message.size());
    memcpy(message.data(), string_message.data(), string_message.size());
	box_socket.send(message);
}
}  // namespace inviwo
