#include "namffplay_usm.cpp"
#include "namffmpeg_usm.cpp"

void NieRAutomataMediaUtil::init_usm(){
    convertOrPlay = new QComboBox;
    convertOrPlay->addItem("Convert (FFmpeg)");
    convertOrPlay->addItem("Playback (FFplay)");
    connect(convertOrPlay, SIGNAL(currentIndexChanged(int)), this, SLOT(convert_or_play_changed(int)));
    formRight->addRow("Action: ", convertOrPlay);
    usm_convert();
}

void NieRAutomataMediaUtil::convert_or_play_changed(int index){
    if(index == 0){
        usm_convert();
    }else if(index == 1){
        usm_playback();
    }
}
