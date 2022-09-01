#include"config.h"

static std::vector<std::string> parse_config_file(std::string path){
    std::ifstream cfg;
    cfg.open(path,std::ios::in);
    if(!cfg.is_open()){
        exit(1);
    }

    std::string line;
    uint32_t num_line=0;
    std::vector<std::string> args;
    args.push_back("program_name");

    while(getline(cfg,line)){
        trim(line);
        num_line++;
        
        if(line[0]=='#' || line.size()==0){
            continue;
        }
        else{
            
            args.push_back("--"+line);
        }
    }
    return args;
}

/* 
    GBR_CONV_VOICE          = 1,
    GBR_CONV_VIDEO          = 2,
    GBR_GAMING              = 3,
    GBR_NON_CONV_VIDEO      = 4,
    GBR_MCPTT               = 65,       // Mission Critical user plane Push To Talk voice
    GBR_NON_MCPTT           = 66,       // Non-Mission-Critical user plane Push To Talk voice
    GBR_MC_VIDEO            = 67,       // Mission Critical Video user plane
    GBR_V2X_MESSAGES        = 75,       // V2X messages
    NGBR_IMS                = 5,
    NGBR_VIDEO_TCP_OPERATOR = 6,
    NGBR_VOICE_VIDEO_GAMING = 7,
    NGBR_VIDEO_TCP_PREMIUM  = 8,
    NGBR_VIDEO_TCP_DEFAULT  = 9,
    NGBR_MC_DELAY_SENSI_SIG = 69,       // Mission Critical delay sensitive signaling (e.g., MC-PTT signaling, MC Video signaling)
    NGBR_MC_DATA            = 70,       // Mission Critical Data (e.g. example services are the same as QCI 6/8/9)
    NGBR_V2X_MESSAGES       = 79,       // V2X messages
    NGBR_LOW_LAT_EMBB_AR    = 80,       // Low latency eMBB applications (TCP/UDP-based); Augmented Reality
    GBR_DISC_AUTO_S_PACKETS = 82,       // Discrete Automation Small Packets (<=255 bytes)
    GBR_DISC_AUTO_B_PACKETS = 83,       // Discrete Automation Big Packets (<=1358 bytes)
    GBR_INTEL_TRANSPORT_SYS = 84,       // Intelligent Transport Systems (<=1358 bytes)
    GBR_ELECTRIC_DIST_HV    = 85,       // Electricity Distribution- high voltage (<=255 bytes)
*/
static int str_to_Qci(std::string s){
    trim(s);
    if(s=="GBR_CONV_VOICE"){
        return 1;
    }
    else if(s=="GBR_CONV_VIDEO")
    {
        return 2;
    }
    else if(s=="GBR_GAMING")
    {
        return 3;
    }
    else if(s=="GBR_NON_CONV_VIDEO")
    {
        return 4;
    }
    else if(s=="GBR_MCPTT")
    {
        return 65;
    }
    else if(s=="GBR_NON_MCPTT")
    {
        return 66;
    }
    else if(s=="GBR_MC_VIDEO")
    {
        return 67;
    }
    else if(s=="GBR_V2X_MESSAGES")
    {
        return 75;
    }
    else if(s=="NGBR_IMS")
    {
        return 5;
    }
    else if(s=="NGBR_VIDEO_TCP_OPERATOR")
    {
        return 6;
    }
    else if(s=="NGBR_VOICE_VIDEO_GAMING")
    {
        return 7;
    }
    else if(s=="NGBR_VIDEO_TCP_PREMIUM")
    {
        return 8;
    }
    else if(s=="NGBR_VIDEO_TCP_DEFAULT")
    {
        return 9;
    }
    else if(s=="NGBR_MC_DELAY_SENSI_SIG")
    {
        return 69;
    }
    else if(s=="NGBR_MC_DATA")
    {
        return 70;
    }
    else if(s=="NGBR_V2X_MESSAGES")
    {
        return 79;
    }
    else if(s=="NGBR_LOW_LAT_EMBB_AR")
    {
        return 80;
    }
    else if(s=="GBR_DISC_AUTO_S_PACKETS")
    {
        return 82;
    }
    else if(s=="GBR_DISC_AUTO_B_PACKETS")
    {
        return 83;
    }
    else if(s=="GBR_INTEL_TRANSPORT_SYS")
    {
        return 84;
    }
    else if(s=="GBR_ELECTRIC_DIST_HV")
    {
        return 85;
    }
    else
    {
        return -1;
    }
}