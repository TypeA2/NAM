#include "nierautomatamediautil.h"
#include "namffmpeg_wem.cpp"

void NieRAutomataMediaUtil::init_wem(){
    formRight->addRow("File conversion:", new QLabel("<i>ww2ogg / FFmpeg</i>"));
    if(info->suffix() == "wem"){
        desc->setText("Audiokinetic Wwise RIFF/RIFX Vorbis audio file.");
        layout_wem();
    }else if(info->suffix() == "wsp"){
        desc->setText("Binary file containing multiple Audiokinetic Wwise RIFF/RIFX Vorbis audio files.");
    }
}
