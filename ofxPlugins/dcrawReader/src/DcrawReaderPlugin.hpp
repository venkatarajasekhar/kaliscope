/* Copyright (C) 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/kaliscope/blob/master/LICENSE
 */

#ifndef _TUTTLE_PLUGIN_DCRAWREADER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DCRAWREADER_PLUGIN_HPP_

#include "DcrawReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <boost/filesystem/path.hpp>

namespace tuttle {
namespace plugin {
namespace dcrawReader {

struct DcrawReaderProcessParams
{
    boost::filesystem::path _filepath;
    int _interpolationQuality;
};

/**
 * @brief DcrawReader plugin
 */
class DcrawReaderPlugin : public ReaderPlugin
{
public:
    DcrawReaderPlugin( OfxImageEffectHandle handle );

public:
    DcrawReaderProcessParams getProcessParams( const OfxTime time ) const;
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
    bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
    void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
    void render( const OFX::RenderArguments &args );

public:
    OFX::ChoiceParam*	_paramInterpQuality;        ///< Interpolation quality
    std::size_t _lastFrame;     ///< Last frame index
};

}
}
}

#endif
