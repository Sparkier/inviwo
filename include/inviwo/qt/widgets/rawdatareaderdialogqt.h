/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2016 Inviwo Foundation
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

#include <inviwo/qt/widgets/inviwoqtwidgetsdefine.h>
#include <inviwo/core/io/datareaderdialog.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <warn/pop>

namespace inviwo {

class IVW_QTWIDGETS_API RawDataReaderDialogQt : public DataReaderDialog, public QDialog {
public:
    RawDataReaderDialogQt();
    virtual ~RawDataReaderDialogQt();

    virtual bool show();
    
    virtual void setFile(std::string fileName);
    
    virtual const DataFormatBase* getFormat() const;
    virtual uvec3 getDimensions() const;
    virtual dvec3 getSpacing() const;
    virtual bool getEndianess() const;
    

private:
    QLabel* fileName_;
    QComboBox* bitDepth_;
    QSpinBox* channels_;
    QSpinBox* dimX_;
    QSpinBox* dimY_;
    QSpinBox* dimZ_;
    
    QLineEdit* spaceX_;
    QLineEdit* spaceY_;
    QLineEdit* spaceZ_;
    
    QSpinBox* timeSteps_;
    QSpinBox* headerOffset_;
    QSpinBox* timeStepOffset_;
    QComboBox* endianess_;
};

} // namespace
