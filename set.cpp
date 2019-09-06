/*
 * Copyright (C) 2019 Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <filesystem>

#include "set.hpp"


Set::Set() : _fileIndex(-1)
{
}

bool Set::addFile(const std::string& fileName, std::string& errorMessage)
{
    File file;
    if (file.init(fileName, errorMessage)) {
        _files.push_back(file);
        return true;
    } else {
        return false;
    }
}

void Set::removeFile(int removeIndex)
{
    if (removeIndex < 0 || removeIndex >= fileCount())
        return;

    if (removeIndex == fileIndex()) {
        std::string tmpErrMsg;
        bool ret;
        if (removeIndex == fileCount() - 1) {
            ret = setFileIndex(fileCount() - 2, tmpErrMsg);
            _files.erase(_files.begin() + removeIndex);
            if (!ret)
                setFileIndex(-1, tmpErrMsg);
        } else {
            std::string tmpErrMsg;
            ret = setFileIndex(fileIndex() + 1, tmpErrMsg);
            _files.erase(_files.begin() + removeIndex);
            if (!ret)
                setFileIndex(-1, tmpErrMsg);
            else
                _fileIndex--;
        }
    } else if (removeIndex < fileIndex()) {
        _files.erase(_files.begin() + removeIndex);
        _fileIndex--;
    } else { // fileIndex() < removeIndex
        _files.erase(_files.begin() + removeIndex);
    }
}

bool Set::setFileIndex(int index, std::string& errorMessage)
{
    if (_fileIndex == index) {
        return true;
    }

    if (index < 0) {
        if (currentFile())
            currentFile()->setFrameIndex(-1, errorMessage); // cannot fail
        _fileIndex = -1;
        return true;
    }

    if (index >= fileCount()) {
        errorMessage = "file " + std::to_string(index) + " does not exist";
        return false;
    }

    int frameIndex = 0;
    int channelIndex = -1;
    if (_fileIndex >= 0) {
        frameIndex = currentFile()->frameIndex();
        if (frameIndex >= 0)
            channelIndex = currentFile()->currentFrame()->channelIndex();
    }

    if (frameIndex < 0) {
        frameIndex = 0;
    } else {
        int frameCount = _files[index].frameCount(errorMessage);
        if (frameCount < 1)
            return false;
        if (frameIndex >= frameCount)
            frameIndex = frameCount - 1;
    }

    if (!_files[index].setFrameIndex(frameIndex, errorMessage))
        return false;

    if (channelIndex != ColorChannelIndex && channelIndex >= _files[index].currentFrame()->channelCount())
        channelIndex = -1;
    if (channelIndex >= 0)
        _files[index].currentFrame()->setChannelIndex(channelIndex);

    if (_fileIndex >= 0 && _fileIndex < fileCount())
        _files[_fileIndex].setFrameIndex(-1, errorMessage);

    _fileIndex = index;
    return true;
}

std::string Set::currentDescription()
{
    std::string desc;
    std::string dummyErrorMsg;
    if (currentFile()) {
        if (fileCount() > 1) {
            desc = std::to_string(fileIndex())
                + '/' + std::to_string(fileCount()) + ' ';
        }
        std::string fileName = std::filesystem::path(currentFile()->fileName()).filename().string();
        desc += fileName + ' ';
        if (currentFile()->frameCount(dummyErrorMsg) > 1) {
            desc += std::to_string(currentFile()->frameIndex())
                + '/' + std::to_string(currentFile()->frameCount(dummyErrorMsg)) + ' ';
        }
        if (currentFile()->currentFrame()->channelCount() > 1) {
            if (currentFile()->currentFrame()->channelIndex() == ColorChannelIndex)
                desc += "color";
            else
                desc += std::string("ch. ") + currentFile()->currentFrame()->currentChannelName()
                    + '/' + std::to_string(currentFile()->currentFrame()->channelCount());
        }
    }
    return desc;
}
