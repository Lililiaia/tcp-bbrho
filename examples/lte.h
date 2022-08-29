#ifndef MY_LTE_EXAMPLE_H
#define MY_LTE_EXAMPLE_H

#include "ns3/core-module.h"
#include"ns3/lte-module.h"
#include"ns3/internet-module.h"
#include"ns3/point-to-point-helper.h"
#include"ns3/network-module.h"
#include"ns3/mobility-module.h"
#include"ns3/applications-module.h"
#include"mybulk-send-helper.h"
#include"my-packet-sink.h"
#include"mybulk-send-application.h"
#include"mypacket-sink-helper.h"
#include"ns3/flow-monitor-module.h"


using namespace ns3;


/*----------------------- Configuration --------------------*/
// TCP related configuration
std::string tcp_type="TcpBbr";
bool window_scaling=true;
bool sack=true;
bool limited_transmit=true;
uint32_t max_window_size=65535;
bool timestamp=true;
Time min_rto=Seconds(1);
Time clock_granularity=MilliSeconds(1);
int re_tx_threshold=3;
int use_ecn=0; // 0 off 1 on 2 AcceptOnly
int snd_buf_size=131072;
int rcv_buf_size=131072;
int tcp_mss=536;
int tcp_initial_cwnd=10;
int tcp_data_retries=6;
int tcp_del_ack_timeout=200;
int tcp_del_ack_count=2;
bool tcp_no_delay=true;
int tcp_persist_timeout=6;
double tcp_rtt_alpha=0.125;
double tcp_rtt_beta=0.25;
int tcp_reduction_bound=1;
DataRate tcp_max_pacing_rate=DataRate("4Gb/s");
int tcp_pacing_ss_ratio=200;
int tcp_pacing_ca_ratio=120;
bool tcp_pace_initial_window=false;
int tcp_bbr_stream=4;
double tcp_bbr_high_gain=2.89;
int tcp_bbr_bw_window_length=10;
Time tcp_bbr_rtt_window_length=Seconds(10.0);
Time tcp_bbr_probe_rtt_duration=MilliSeconds(200.0);
int tcp_bbr_extra_ack_rtt_window_length=5;
int tcp_bbr_ack_epoch_acked_reset_thresh=4096;

int lte_eps_bearer_to_rlc_mapping=1;
int lte_system_information_periodicity=5;
int lte_src_periodicity=320;
int lte_connection_request_timeout_duration=15;
int lte_connection_setup_timeout_duration=150;
int lte_connection_rejected_timeout_duration=30;
int lte_handover_joining_timeout_duration=45;
int lte_handover_leaving_timeout_duration=45;
int lte_outage_threshold=-5;
int lte_qrx_level_min=-70;
bool lte_admit_handover_request=true;
bool lte_admit_rrc_connection_request=true;
int lte_rsrp_filter_coefficient=4;
bool lte_mm_wave_device=false;
int lte_first_sib_time=2;
bool lte_inter_rat_ho_mode=false;
int lte_ho_sinr_difference=3;
int lte_secondary_cell_handover_mode=2;
int lte_fixed_ttt_value=110;
int lte_minimum_ttt_value=25;
int lte_max_diff_value=20;
int lte_min_diff_value=3;
int lte_crt_period=1600;
bool lte_report_all_ue_meas=true;
bool enable_handover_trace=false;

DataRate lte_s1u_link_data_rate=DataRate("10Gb/s");

Time lte_s1u_link_delay=MilliSeconds(0.0);

int lte_s1u_link_mtu=2000;

DataRate lte_s1ap_link_data_rate=DataRate("10Mb/s");

Time lte_s1sp_link_delay=MilliSeconds(10.0);

int lte_s1ap_link_mtu=2000;

DataRate lte_x2_link_date_rate=DataRate("10Gb/s");

Time lte_x2_link_delay=MilliSeconds(0);

int lte_x2_link_mtu=3000;

std::string root_dir="../results/";

int simulation_time=200;

std::string lte_path_loss_model_type="FriisPropagationLossModel";
std::string lte_scheduler="PfFfMacScheduler";
std::string lte_ffr_algorithm="LteFrNoOpAlgorithm";
std::string lte_handover_algorithm="NoOpHandoverAlgorithm";
bool lte_use_ideal_rrc=true;
bool lte_anr_enabled=true;
bool lte_use_pdsch_for_cqi_generation=true;
std::string lte_enb_component_carrier_manager="NoOpComponentCarrierManager";
std::string lte_ue_component_carrier_manager="SimpleUeComponentCarrierManager";
bool lte_use_ca=false;
int lte_num_of_component_carriers=1;

DataRate link1_data_rate=DataRate("10Mb/s");
Time link1_delay=MilliSeconds(10.0);
int link1_mtu=1500;

int send_size=512;


/* -------- trace or stats ---------------=------------*/
bool enable_rto_trace=false;
bool enable_rtt_trace=false;
bool enable_advwnd_trace=false;
bool enable_rwnd_trace=false;
bool enable_pacing_rate_trace=false;
bool enable_cwnd_trace=true;
bool enable_cwnd_inflate_trace=false;
bool enable_tx_throughput_stats=true;
bool enable_rx_throughput_stats=true;
bool enable_pcap=false;

bool enable_packet_drop_stats=false;

/* 
    packet drop stats
*/
// a packet has been dropped by the device before transmission
// potential causes
// 1. link is down
// 2. tx queue is full
std::vector<uint64_t> mac_drop;
// a packet has been dropped by the device during transmission
std::vector<uint64_t> phy_tx_drop;
// a packet has been dropped by the device during reception
std::vector<uint64_t> phy_rx_drop;
std::vector<std::string> device_name;

uint64_t tx_bytes=0;
uint64_t rx_bytes=0;


static void print_config();
static void default_config();
static void mac_tx_drop_callback(int device_id,Ptr<const Packet>);
static void phy_tx_drop_callback(int device_id,Ptr<const Packet>);
static void phy_rx_drop_callback(int device_id,Ptr<const Packet>);
static void print_stats();
static int get_device_id(std::string name);
static void rto_trace(Time old_val, Time new_val);
static void rtt_trace(Time old_val,Time new_val);
static void advwnd_trace(uint32_t old_val,uint32_t new_val);
static void cwnd_trace(uint32_t old_val, uint32_t new_val);
static void cwnd_inflated_trace(uint32_t old_val, uint32_t new_val);
static void tx_trace(Ptr<const Packet> p, const TcpHeader& header,Ptr<const TcpSocketBase> socket);
static void rx_trace(Ptr<const Packet>p, const Address & addr);
static void serverConnectedSucceeded(Ptr<Socket> skt);
static void connectedFailed(Ptr<Socket> skt);

static void mac_tx_drop_callback(int device_id,Ptr<const Packet> p){
    mac_drop[device_id]++;
}
static void phy_tx_drop_callback(int device_id,Ptr<const Packet> p){
    phy_tx_drop[device_id]++;
}
static void phy_rx_drop_callback(int device_id,Ptr<const Packet> p){
    phy_rx_drop[device_id]++;
}

static void print_stats(){
    std::ofstream thr(root_dir + "stats.data", std::ios::out | std::ios::app);
    if(enable_packet_drop_stats){
        int len=mac_drop.size();
        for(int i=0;i<len;++i){
            thr<< device_name[i] << ": "<<std::endl;
            thr<<"      mac tx drop:"<<mac_drop[i]<<std::endl;
            thr<<"      phy tx drop:"<<phy_tx_drop[i]<<std::endl;
            thr<<"      phy rx drop:"<<phy_rx_drop[i]<<std::endl;
        }
    }
}

static int get_device_id(std::string name){
    mac_drop.push_back(0);
    phy_rx_drop.push_back(0);
    phy_tx_drop.push_back(0);
    device_name.push_back(name);
    return mac_drop.size()-1;
}


// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

static void rto_trace(Time old_val, Time new_val){
    static std::ofstream thr(root_dir + "rto.trace", std::ios::out | std::ios::app);
    Time curtime=Now();
    thr<<curtime << " " << old_val << " "<<new_val<<std::endl;
}

static void rtt_trace(Time old_val, Time new_val){
    static std::ofstream thr(root_dir + "rtt.trace", std::ios::out | std::ios::app);
    Time curtime=Now();
    thr<<curtime << " " << old_val << " "<<new_val<<std::endl;
}

static void advwnd_trace(uint32_t old_val, uint32_t new_val){
    static std::ofstream thr(root_dir + "advwnd.trace", std::ios::out | std::ios::app);
    Time curtime=Now();
    thr<<curtime << " " << old_val << " "<<new_val<<std::endl;
}

static void cwnd_trace(uint32_t old_val, uint32_t new_val){
    static std::ofstream thr(root_dir + "cwnd.trace", std::ios::out | std::ios::app);
    Time curtime=Now();
    thr<<curtime << " " << old_val/tcp_mss << " "<<new_val/tcp_mss<<std::endl;
}

static void cwnd_inflated_trace(uint32_t old_val, uint32_t new_val){
    static std::ofstream thr(root_dir + "cwnd_inflated.trace", std::ios::out | std::ios::app);
    Time curtime=Now();
    thr<<curtime << " " << old_val/tcp_mss << " "<<new_val/tcp_mss<<std::endl;
}
static void tx_trace(Ptr<const Packet> p, const TcpHeader& header,Ptr<const TcpSocketBase> socket){
    tx_bytes+=p->GetSize();
}

static void rx_trace(Ptr<const Packet>p, const Address & addr){
    rx_bytes+=p->GetSize();
}

static void serverConnectedSucceeded(Ptr<Socket> skt){
    Ptr<TcpSocketBase> sSocket=DynamicCast<TcpSocketBase,Socket>(skt);
    NS_ASSERT(sSocket!=0);
    if(enable_rto_trace){
        sSocket->TraceConnectWithoutContext("RTO",MakeCallback(&rto_trace));
    }

    if(enable_rtt_trace){
        sSocket->TraceConnectWithoutContext("RTT",MakeCallback(&rtt_trace));
    }

    if(enable_advwnd_trace){
        sSocket->TraceConnectWithoutContext("AdvWND",MakeCallback(&advwnd_trace));
    }    

    if(enable_cwnd_trace){
        sSocket->TraceConnectWithoutContext("CongestionWindow",MakeCallback(&cwnd_trace));
    }

    if(enable_cwnd_inflate_trace){
        sSocket->TraceConnectWithoutContext("CongestionWindowInflated",MakeCallback(&cwnd_inflated_trace));
    }
    
    if(enable_tx_throughput_stats){
        sSocket->TraceConnectWithoutContext("Tx",MakeCallback(&tx_trace));
    }
}


static void connectedFailed(Ptr<Socket> skt){
    NS_ABORT_MSG("connected failure");
}



static void print_config(){
    NS_LOG_UNCOND("tcp_type " <<tcp_type);
    NS_LOG_UNCOND("window_scaling " <<window_scaling);
    NS_LOG_UNCOND("sack " <<sack);
    NS_LOG_UNCOND("limited_transmit " <<limited_transmit);
    NS_LOG_UNCOND("max_window_size " <<max_window_size);
    NS_LOG_UNCOND("timestamp " <<timestamp);
    NS_LOG_UNCOND("min_rto " <<min_rto);
    NS_LOG_UNCOND("clock_granularity " <<clock_granularity);
    NS_LOG_UNCOND("re_tx_threshold " <<re_tx_threshold);
    NS_LOG_UNCOND("use_ecn " <<use_ecn);
    NS_LOG_UNCOND("enable_rto_trace" <<enable_rto_trace);
    NS_LOG_UNCOND("enable_rtt_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_advwnd_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_rwnd_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_pacing_rate_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_cwnd_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_cwnd_inflate_trace " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_tx_throughput_stats " <<enable_rto_trace);
    NS_LOG_UNCOND("enable_rx_throughput_stats " <<enable_rto_trace);

    NS_LOG_UNCOND("snd_buf_size " <<snd_buf_size);
    NS_LOG_UNCOND("rcv_buf_size " <<rcv_buf_size);
    NS_LOG_UNCOND("tcp_mss " <<tcp_mss);
    NS_LOG_UNCOND("tcp_initial_cwnd " <<tcp_initial_cwnd);
    NS_LOG_UNCOND("tcp_data_retries " <<tcp_data_retries);
    NS_LOG_UNCOND("tcp_del_ack_timeout " <<tcp_del_ack_timeout);
    NS_LOG_UNCOND("tcp_del_ack_count " <<tcp_del_ack_count);
    NS_LOG_UNCOND("tcp_no_delay " <<tcp_no_delay);
    NS_LOG_UNCOND("tcp_persist_timeout " <<tcp_persist_timeout);

    NS_LOG_UNCOND("tcp_rtt_alpha " <<tcp_rtt_alpha);
    NS_LOG_UNCOND("tcp_rtt_beta " <<tcp_rtt_beta);
    NS_LOG_UNCOND("tcp_reduction_bound " <<tcp_reduction_bound);

    NS_LOG_UNCOND("tcp_max_pacing_rate " <<tcp_max_pacing_rate);
    NS_LOG_UNCOND("tcp_pacing_ss_ratio " <<tcp_pacing_ss_ratio);
    NS_LOG_UNCOND("tcp_pacing_ca_ratio " <<tcp_pacing_ca_ratio);
    NS_LOG_UNCOND("tcp_pace_initial_window " <<tcp_pace_initial_window);

    NS_LOG_UNCOND("tcp_bbr_stream " <<tcp_bbr_stream);
    NS_LOG_UNCOND("tcp_bbr_high_gain " <<tcp_bbr_high_gain);
    NS_LOG_UNCOND("tcp_bbr_bw_window_length " <<tcp_bbr_bw_window_length);
    NS_LOG_UNCOND("tcp_bbr_rtt_window_length " <<tcp_bbr_rtt_window_length);
    NS_LOG_UNCOND("tcp_bbr_probe_rtt_duration " <<tcp_bbr_probe_rtt_duration);
    NS_LOG_UNCOND("tcp_bbr_extra_ack_rtt_window_length " <<tcp_bbr_extra_ack_rtt_window_length);
    NS_LOG_UNCOND("tcp_bbr_ack_epoch_acked_reset_thresh " <<tcp_bbr_ack_epoch_acked_reset_thresh);

}


/* // for string delimiter
static std::vector<std::string> split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
} */


static std::vector<std::string> parse_config_file(std::string file){
    std::ifstream cfg;
    cfg.open(file,std::ios::in);
    if(!cfg.is_open()){
        NS_ABORT_MSG("can not open configuration file : config.cfg");
    }

    std::string line;
    uint32_t num_line=0;
    std::vector<std::string> args;

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
    int len=args.size();
    for(int i=0;i<len;i++){
        NS_LOG_UNCOND(args[i]);
    }
    return args;
}

static void config_init(int argv,char* argc[]){
    if(argv !=2) {
        NS_ABORT_MSG("please give the configuration file's path");
    }
    auto args=parse_config_file(argc[1]);

    CommandLine cmd;
    int _min_rto=1;
    int _clock_granularity=1;
    std::string _use_ecn="off";
    std::string _tcp_rtt_alpha="0.125";
    std::string _tcp_rtt_beta="0.25";
    std::string _tcp_reduction_bound="SSRB";
    std::string _tcp_max_pacing_rate="4Gb/s";
    
    cmd.AddValue("tcp_type","TCP TYPE eg: TcpBbr TcpCubic TcpNewReno TcpBic",tcp_type);
    cmd.AddValue("enable_windowscaling_option","enable or disable window scaling option",window_scaling);
    cmd.AddValue("enable_sack_option","enable or disable sack option",sack);
    cmd.AddValue("enable_limited_transmit","enable or distable limited transmit",limited_transmit);
    cmd.AddValue("max_window_size","max size of advertised window",max_window_size);
    cmd.AddValue("enable_ts_option","enable or disable timestamp option",timestamp);
    cmd.AddValue("min_rto","minimum retransmit timeout value: (second).",_min_rto);
    cmd.AddValue("clock_granularity","clock granularity used in rto calculations (ms)",_clock_granularity);
    cmd.AddValue("re_tx_threshold","Threshold for fast retransmit",re_tx_threshold);
    cmd.AddValue("use_ecn","ECN functionality (Off|On|AcceptOnly)",_use_ecn);
    cmd.AddValue("enable_rto_trace","enable the statistic of retransmission timeout",enable_rto_trace);

    cmd.AddValue("enable_rtt_trace","enable RTT sample trace",enable_rtt_trace);
    cmd.AddValue("enable_advwnd_trace","enable Advertised Window Size trace",enable_advwnd_trace);
    cmd.AddValue("enable_rwnd_trace","Remote side's flow control window trace",enable_rwnd_trace);
    cmd.AddValue("enable_pacing_rate_trace","The current TCP pacing rate trace",enable_pacing_rate_trace);
    cmd.AddValue("enable_cwnd_trace","The TCP connection's congestion window trace",enable_cwnd_trace);
    cmd.AddValue("enable_cwnd_inflate_trace","The TCP connection's congestion window inflates as in older RFC",enable_cwnd_inflate_trace);
    cmd.AddValue("enable_tx_throughput_stats","Tx throughput stats",enable_tx_throughput_stats);
    cmd.AddValue("enable_rx_throughput_stats","Rx throughput stats",enable_rx_throughput_stats);

    cmd.AddValue("snd_buf_size","TcpSocket maximum transmit buffer size (128K)",snd_buf_size);
    cmd.AddValue("rcv_buf_size","TcpSocket maximum receive buffer size (128K)",rcv_buf_size);
    cmd.AddValue("tcp_mss","Tcp maximum segmenet size (bytes)",tcp_mss);
    cmd.AddValue("tcp_initial_cwnd","TCP initial congestion window size(segments) (10)",tcp_initial_cwnd);
    cmd.AddValue("tcp_data_retries","Number of data retransmission attempts(6)",tcp_data_retries);
    cmd.AddValue("tcp_del_ack_timeout","Timeout value for TCP delayed acks (ms)",tcp_del_ack_timeout);
    cmd.AddValue("tcp_del_ack_count","number of packets to wait before sending a TCP ack",tcp_del_ack_count);
    cmd.AddValue("tcp_no_delay","set to true to disable Nagle's algorithm",tcp_no_delay);
    cmd.AddValue("tcp_persist_timeout","persist timeout to probe for rx window",tcp_persist_timeout);
    cmd.AddValue("tcp_rtt_alpha","persist timeout to probe for rx window",_tcp_rtt_alpha);
    cmd.AddValue("tcp_rtt_beta","persist timeout to probe for rx window",_tcp_rtt_beta);
    cmd.AddValue("tcp_reduction_bound","persist timeout to probe for rx window",_tcp_reduction_bound);

    cmd.AddValue("tcp_max_pacing_rate","persist timeout to probe for rx window",_tcp_max_pacing_rate);
    cmd.AddValue("tcp_pacing_ss_ratio","persist timeout to probe for rx window",tcp_pacing_ss_ratio);
    cmd.AddValue("tcp_pacing_ca_ratio","persist timeout to probe for rx window",tcp_pacing_ca_ratio);
    cmd.AddValue("tcp_pace_initial_window","persist timeout to probe for rx window",tcp_pace_initial_window);
    std::string _tcp_bbr_high_gain="2.89";
    int _tcp_bbr_rtt_window_length=10;
    int _tcp_bbr_probe_rtt_duration=200;
    cmd.AddValue("tcp_bbr_stream","persist timeout to probe for rx window",tcp_bbr_stream);
    cmd.AddValue("tcp_bbr_high_gain","persist timeout to probe for rx window",_tcp_bbr_high_gain);
    cmd.AddValue("tcp_bbr_bw_window_length","persist timeout to probe for rx window",tcp_bbr_bw_window_length);
    cmd.AddValue("tcp_bbr_rtt_window_length","persist timeout to probe for rx window",_tcp_bbr_rtt_window_length);
    cmd.AddValue("tcp_bbr_probe_rtt_duration","persist timeout to probe for rx window",_tcp_bbr_probe_rtt_duration);
    cmd.AddValue("tcp_bbr_extra_ack_rtt_window_length","persist timeout to probe for rx window",tcp_bbr_extra_ack_rtt_window_length);
    cmd.AddValue("tcp_bbr_ack_epoch_acked_reset_thresh","persist timeout to probe for rx window",tcp_bbr_ack_epoch_acked_reset_thresh);

    cmd.AddValue("lte_eps_bearer_to_rlc_mapping","",lte_eps_bearer_to_rlc_mapping);
    cmd.AddValue("lte_system_information_periodicity","",lte_system_information_periodicity);
    cmd.AddValue("lte_src_periodicity","",lte_src_periodicity);
    cmd.AddValue("lte_connection_request_timeout_duration","",lte_connection_request_timeout_duration);
    cmd.AddValue("lte_connection_setup_timeout_duration","",lte_connection_setup_timeout_duration);
    cmd.AddValue("lte_connection_rejected_timeout_duration","",lte_connection_rejected_timeout_duration);
    cmd.AddValue("lte_handover_joining_timeout_duration","",lte_handover_joining_timeout_duration);
    cmd.AddValue("lte_handover_leaving_timeout_duration","",lte_handover_leaving_timeout_duration);
    cmd.AddValue("lte_outage_threshold","",lte_outage_threshold);
    cmd.AddValue("lte_qrx_level_min","",lte_qrx_level_min);
    cmd.AddValue("lte_admit_handover_request","",lte_admit_handover_request);
    cmd.AddValue("lte_admit_rrc_connection_request","",lte_admit_rrc_connection_request);
    cmd.AddValue("lte_rsrp_filter_coefficient","",lte_rsrp_filter_coefficient);
    cmd.AddValue("lte_mm_wave_device","",lte_mm_wave_device);
    cmd.AddValue("lte_first_sib_time","",lte_first_sib_time);
    cmd.AddValue("lte_inter_rat_ho_mode","",lte_inter_rat_ho_mode);
    cmd.AddValue("lte_ho_sinr_difference","",lte_ho_sinr_difference);
    cmd.AddValue("lte_secondary_cell_handover_mode","",lte_secondary_cell_handover_mode);
    cmd.AddValue("lte_fixed_ttt_value","",lte_fixed_ttt_value);
    cmd.AddValue("lte_minimum_ttt_value","",lte_minimum_ttt_value);
    cmd.AddValue("lte_max_diff_value","",lte_max_diff_value);
    cmd.AddValue("lte_min_diff_value","",lte_min_diff_value);
    cmd.AddValue("lte_crt_period","",lte_crt_period);
    cmd.AddValue("lte_report_all_ue_meas","",lte_report_all_ue_meas);
    cmd.AddValue("enable_handover_trace","",enable_handover_trace);

    std::string _lte_s1u_link_data_rate="10Gb/s";
    std::string _lte_s1ap_link_data_rate="10Mb/s";
    std::string _lte_x2_link_date_rate="10Gb/s";
    std::string _lte_s1u_link_delay="0.0";
    std::string _lte_s1sp_link_delay="10";
    std::string _lte_x2_link_delay="0";
    cmd.AddValue("lte_s1u_link_data_rate","",_lte_s1u_link_data_rate);
    cmd.AddValue("lte_s1u_link_delay","",_lte_s1u_link_delay);
    cmd.AddValue("lte_s1u_link_mtu","",lte_s1u_link_mtu);
    cmd.AddValue("lte_s1ap_link_data_rate","",_lte_s1ap_link_data_rate);
    cmd.AddValue("lte_s1sp_link_delay","",_lte_s1sp_link_delay);
    cmd.AddValue("lte_s1ap_link_mtu","",lte_s1ap_link_mtu);
    cmd.AddValue("lte_x2_link_date_rate","",_lte_x2_link_date_rate);
    cmd.AddValue("lte_x2_link_delay","",_lte_x2_link_delay);
    cmd.AddValue("lte_x2_link_mtu","",lte_x2_link_mtu);
    cmd.AddValue("enable_pcap","",enable_pcap);
    cmd.AddValue("root_dir","",root_dir);
    
    cmd.AddValue("lte_path_loss_model_type","",lte_path_loss_model_type);
    cmd.AddValue("lte_scheduler","",lte_scheduler);
    cmd.AddValue("lte_ffr_algorithm","",lte_ffr_algorithm);
    cmd.AddValue("lte_handover_algorithm","",lte_handover_algorithm);
    cmd.AddValue("lte_use_ideal_rrc","",lte_use_ideal_rrc);
    cmd.AddValue("lte_anr_enabled","",lte_anr_enabled);
    cmd.AddValue("lte_use_pdsch_for_cqi_generation","",lte_use_pdsch_for_cqi_generation);
    cmd.AddValue("lte_enb_component_carrier_manager","",lte_enb_component_carrier_manager);
    cmd.AddValue("lte_ue_component_carrier_manager","",lte_ue_component_carrier_manager);
    cmd.AddValue("lte_use_ca","",lte_use_ca);
    cmd.AddValue("lte_num_of_component_carriers","",lte_num_of_component_carriers);
    
    std::string _link1_data_rate="10Mb/s";
    std::string _link1_delay="10";
    cmd.AddValue("link1_data_rate","",_link1_data_rate);
    cmd.AddValue("link1_delay","",_link1_delay);
    cmd.AddValue("link1_mtu","",link1_mtu);
    cmd.AddValue("enable_packet_drop_stats","",enable_packet_drop_stats);
    
    cmd.AddValue("simulation_time","",simulation_time);
    cmd.AddValue("send_size","",send_size);


    cmd.Parse(args);

    min_rto=Seconds(_min_rto);
    clock_granularity=MilliSeconds(_clock_granularity);
    transform(_use_ecn.begin(), _use_ecn.end(), _use_ecn.begin(), ::tolower);
    if(_use_ecn=="off"){
        use_ecn=0;
    }else if(_use_ecn=="on"){
        use_ecn=1;
    }else if(_use_ecn=="acceptonly"){
        use_ecn=2;
    }else{
        NS_ABORT_MSG("Invalid arguments: use_ecn="<<use_ecn);
    }

    tcp_rtt_alpha=std::stod(_tcp_rtt_alpha);
    tcp_rtt_beta=std::stod(_tcp_rtt_beta);
    transform(_tcp_reduction_bound.begin(), _tcp_reduction_bound.end(), _tcp_reduction_bound.begin(), ::toupper);

    if(_tcp_reduction_bound.compare("SSRB")==0){
         tcp_reduction_bound=1;
    }else if (_tcp_reduction_bound.compare("CRB")==0){
        tcp_reduction_bound=0;
    }else{
        NS_ABORT_MSG("Invalid arguments: tcp_reduction_bound="<<_tcp_reduction_bound);
    }

    tcp_max_pacing_rate=DataRate(_tcp_max_pacing_rate);

    tcp_bbr_high_gain=std::stod(_tcp_bbr_high_gain);
    tcp_bbr_rtt_window_length=Seconds(_tcp_bbr_rtt_window_length);
    tcp_bbr_probe_rtt_duration=MilliSeconds(_tcp_bbr_probe_rtt_duration);
    lte_s1u_link_data_rate=DataRate(_lte_s1u_link_data_rate);
    lte_s1u_link_delay=MilliSeconds(std::stod(_lte_s1u_link_delay));
    lte_s1ap_link_data_rate=DataRate(_lte_s1ap_link_data_rate);
    lte_s1sp_link_delay=MilliSeconds(std::stod(_lte_s1sp_link_delay));
    lte_x2_link_date_rate=DataRate(_lte_x2_link_date_rate);
    lte_x2_link_delay=MilliSeconds(std::stod(_lte_x2_link_delay));

    link1_data_rate=DataRate(_link1_data_rate);
    link1_delay=MilliSeconds(std::stod(_link1_delay));

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buffer, sizeof (buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    std::string currentTime (buffer);

    root_dir+=currentTime+"/";
    std::string dirToSave = "mkdir -p " + root_dir;
    if (system (dirToSave.c_str ()) == -1)
    {
        exit (1);
    }

    print_config();
}

static void default_config(){
    // TcpSocketBase
    Config::SetDefault("ns3::TcpSocketBase::MaxWindowSize",UintegerValue(max_window_size));
    Config::SetDefault("ns3::TcpSocketBase::WindowScaling",BooleanValue(window_scaling));
    Config::SetDefault("ns3::TcpSocketBase::Sack",BooleanValue(sack));
    Config::SetDefault("ns3::TcpSocketBase::Timestamp",BooleanValue(timestamp));
    Config::SetDefault("ns3::TcpSocketBase::MinRto",TimeValue(min_rto));
    Config::SetDefault("ns3::TcpSocketBase::ClockGranularity",TimeValue(clock_granularity));
    Config::SetDefault("ns3::TcpSocketBase::ReTxThreshold",UintegerValue(re_tx_threshold));
    Config::SetDefault("ns3::TcpSocketBase::LimitedTransmit",BooleanValue(limited_transmit));
    Config::SetDefault("ns3::TcpSocketBase::UseEcn",EnumValue(use_ecn));

    //TcpSocket
    Config::SetDefault("ns3::TcpSocket::SndBufSize",UintegerValue(snd_buf_size));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize",UintegerValue(rcv_buf_size));
    Config::SetDefault("ns3::TcpSocket::SegmentSize",UintegerValue(tcp_mss));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd",UintegerValue(tcp_initial_cwnd));
    Config::SetDefault("ns3::TcpSocket::DataRetries",UintegerValue(tcp_data_retries));
    Config::SetDefault("ns3::TcpSocket::DelAckTimeout",TimeValue(MilliSeconds(tcp_del_ack_timeout)));
    Config::SetDefault("ns3::TcpSocket::DelAckCount",UintegerValue(tcp_del_ack_count));
    Config::SetDefault("ns3::TcpSocket::TcpNoDelay",BooleanValue(tcp_no_delay));
    Config::SetDefault("ns3::TcpSocket::PersistTimeout",TimeValue(Seconds(tcp_persist_timeout)));

    //TcpL4Protocol
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::"+tcp_type));

    //TcpPrrRecovery
    Config::SetDefault ("ns3::TcpPrrRecovery::ReductionBound", EnumValue(tcp_reduction_bound));

    //RttMeanDeviation
    Config::SetDefault("ns3::RttMeanDeviation::Alpha",DoubleValue(tcp_rtt_alpha));
    Config::SetDefault("ns3::RttMeanDeviation::Beta",DoubleValue(tcp_rtt_beta));

    //TcpSocketState
    Config::SetDefault("ns3::TcpSocketState::EnablePacing",BooleanValue(enable_pacing_rate_trace));
    Config::SetDefault("ns3::TcpSocketState::MaxPacingRate",DataRateValue(tcp_max_pacing_rate));
    Config::SetDefault("ns3::TcpSocketState::PacingSsRatio",UintegerValue(tcp_pacing_ss_ratio));
    Config::SetDefault("ns3::TcpSocketState::PacingCaRatio",UintegerValue(tcp_pacing_ca_ratio));
    Config::SetDefault("ns3::TcpSocketState::PaceInitialWindow",BooleanValue(tcp_pace_initial_window));

    //TcpBbr
    Config::SetDefault("ns3::TcpBbr::Stream",UintegerValue(tcp_bbr_stream));
    Config::SetDefault("ns3::TcpBbr::HighGain",DoubleValue(tcp_bbr_high_gain));
    Config::SetDefault("ns3::TcpBbr::BwWindowLength",UintegerValue(tcp_bbr_bw_window_length));
    Config::SetDefault("ns3::TcpBbr::RttWindowLength",TimeValue(tcp_bbr_rtt_window_length));
    Config::SetDefault("ns3::TcpBbr::ProbeRttDuration",TimeValue(tcp_bbr_probe_rtt_duration));
    Config::SetDefault("ns3::TcpBbr::ExtraAckedRttWindowLength",UintegerValue(tcp_bbr_extra_ack_rtt_window_length));
    Config::SetDefault("ns3::TcpBbr::AckEpochAckedResetThresh",UintegerValue(tcp_bbr_ack_epoch_acked_reset_thresh));


    // LTE
    Config::SetDefault("ns3::LteEnbRrc::EpsBearerToRlcMapping",EnumValue(lte_eps_bearer_to_rlc_mapping));
    Config::SetDefault("ns3::LteEnbRrc::SystemInformationPeriodicity",TimeValue (MilliSeconds (lte_system_information_periodicity)));
    Config::SetDefault("ns3::LteEnbRrc::SrsPeriodicity",UintegerValue(lte_src_periodicity));
    Config::SetDefault("ns3::LteEnbRrc::ConnectionRequestTimeoutDuration",TimeValue(MilliSeconds(lte_connection_request_timeout_duration)));
    Config::SetDefault("ns3::LteEnbRrc::ConnectionSetupTimeoutDuration",TimeValue(MilliSeconds(lte_connection_setup_timeout_duration)));
    Config::SetDefault("ns3::LteEnbRrc::ConnectionRejectedTimeoutDuration",TimeValue(MilliSeconds(lte_connection_rejected_timeout_duration)));
    Config::SetDefault("ns3::LteEnbRrc::HandoverJoiningTimeoutDuration",TimeValue(Seconds(lte_handover_joining_timeout_duration)));
    Config::SetDefault("ns3::LteEnbRrc::HandoverLeavingTimeoutDuration",TimeValue(Seconds(lte_handover_leaving_timeout_duration)));
    Config::SetDefault("ns3::LteEnbRrc::OutageThreshold",DoubleValue(lte_outage_threshold));
    Config::SetDefault("ns3::LteEnbRrc::QRxLevMin",IntegerValue(lte_qrx_level_min));
    Config::SetDefault("ns3::LteEnbRrc::AdmitHandoverRequest",BooleanValue(lte_admit_handover_request));
    Config::SetDefault("ns3::LteEnbRrc::AdmitRrcConnectionRequest",BooleanValue(lte_admit_rrc_connection_request));
    Config::SetDefault("ns3::LteEnbRrc::RsrpFilterCoefficient",UintegerValue(lte_rsrp_filter_coefficient));
    Config::SetDefault("ns3::LteEnbRrc::mmWaveDevice",BooleanValue(lte_mm_wave_device));
    Config::SetDefault("ns3::LteEnbRrc::FirstSibTime",UintegerValue(lte_first_sib_time));
    Config::SetDefault("ns3::LteEnbRrc::InterRatHoMode",BooleanValue(lte_inter_rat_ho_mode));
    Config::SetDefault("ns3::LteEnbRrc::HoSinrDifference",DoubleValue(lte_ho_sinr_difference));
    Config::SetDefault("ns3::LteEnbRrc::SecondaryCellHandoverMode",EnumValue(lte_secondary_cell_handover_mode));
    Config::SetDefault("ns3::LteEnbRrc::FixedTttValue",UintegerValue(lte_fixed_ttt_value));
    Config::SetDefault("ns3::LteEnbRrc::MinDynTttValue",UintegerValue(lte_minimum_ttt_value));
    Config::SetDefault("ns3::LteEnbRrc::MinDiffValue",DoubleValue(lte_min_diff_value));
    Config::SetDefault("ns3::LteEnbRrc::MaxDiffValue",DoubleValue(lte_max_diff_value));
    Config::SetDefault("ns3::LteEnbRrc::CrtPeriod",IntegerValue(lte_crt_period));
    Config::SetDefault("ns3::LteEnbRrc::ReportAllUeMeas",BooleanValue(lte_report_all_ue_meas));

    //PointToPointEpcHelper
    Config::SetDefault("ns3::PointToPointEpcHelper::S1uLinkDataRate",DataRateValue(lte_s1u_link_data_rate));
    Config::SetDefault("ns3::PointToPointEpcHelper::S1apLinkDataRate",DataRateValue(lte_s1ap_link_data_rate));
    Config::SetDefault("ns3::PointToPointEpcHelper::X2LinkDataRate",DataRateValue(lte_x2_link_date_rate));
    Config::SetDefault("ns3::PointToPointEpcHelper::S1uLinkDelay",TimeValue(lte_s1u_link_delay));
    Config::SetDefault("ns3::PointToPointEpcHelper::S1apLinkDelay",TimeValue(lte_s1sp_link_delay));
    Config::SetDefault("ns3::PointToPointEpcHelper::X2LinkDelay",TimeValue(lte_x2_link_delay));
    Config::SetDefault("ns3::PointToPointEpcHelper::S1uLinkMtu",UintegerValue(lte_s1u_link_mtu));
    Config::SetDefault("ns3::PointToPointEpcHelper::S1apLinkMtu",UintegerValue(lte_s1ap_link_mtu));
    Config::SetDefault("ns3::PointToPointEpcHelper::X2LinkMtu",UintegerValue(lte_x2_link_mtu));
    if(enable_pcap){
        Config::SetDefault("ns3::PointToPointEpcHelper::S1uLinkEnablePcap",BooleanValue(true));
        Config::SetDefault("ns3::PointToPointEpcHelper::X2LinkEnablePcap",BooleanValue(true));
        Config::SetDefault("ns3::PointToPointEpcHelper::S1uLinkPcapPrefix",StringValue(root_dir+"s1-u"));
        Config::SetDefault("ns3::PointToPointEpcHelper::X2LinkPcapPrefix",StringValue(root_dir+"x2"));
        Config::SetDefault("ns3::RadioBearerStatsCalculator::DlRlcOutputFilename",StringValue(root_dir+"DlRlcStats.txt"));
        Config::SetDefault("ns3::RadioBearerStatsCalculator::UlRlcOutputFilename",StringValue(root_dir+"UlRlcStats.txt"));
        Config::SetDefault("ns3::RadioBearerStatsCalculator::DlPdcpOutputFilename",StringValue(root_dir+"DlPdcpStats.txt"));
        Config::SetDefault("ns3::RadioBearerStatsCalculator::UlPdcpOutputFilename",StringValue(root_dir+"UlPdcpStats.txt"));
    }

    // LteHelper
    Config::SetDefault("ns3::LteHelper::Scheduler",StringValue("ns3::"+lte_scheduler));
    Config::SetDefault("ns3::LteHelper::FfrAlgorithm",StringValue("ns3::"+lte_ffr_algorithm));
    Config::SetDefault("ns3::LteHelper::HandoverAlgorithm",StringValue("ns3::"+lte_handover_algorithm));
    Config::SetDefault("ns3::LteHelper::UseIdealRrc",BooleanValue(lte_use_ideal_rrc));
    Config::SetDefault("ns3::LteHelper::AnrEnabled",BooleanValue(lte_anr_enabled));
    Config::SetDefault("ns3::LteHelper::UsePdschForCqiGeneration",BooleanValue(lte_use_pdsch_for_cqi_generation));
    Config::SetDefault("ns3::LteHelper::EnbComponentCarrierManager",StringValue("ns3::"+lte_enb_component_carrier_manager));
    Config::SetDefault("ns3::LteHelper::UeComponentCarrierManager",StringValue("ns3::"+lte_ue_component_carrier_manager));
    Config::SetDefault("ns3::LteHelper::UseCa",BooleanValue(lte_use_ca));
    Config::SetDefault("ns3::LteHelper::NumberOfComponentCarriers",UintegerValue(lte_num_of_component_carriers));
}


/* ---------------- statistic ------------------------ */


#endif