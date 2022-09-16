#include <fstream>
#include <string>
#include <iomanip>
#include "ns3/core-module.h"
#include "ns3/mmwave-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "../config.h"
#include "../mybulk-send-helper.h"
#include "../mybulk-send-application.h"

using namespace ns3;
using namespace mmwave;
NS_LOG_COMPONENT_DEFINE("MmWaveTcpX2Handover");

/* 
     ************************Function Declaration Start********************************
*/
void init_config();
Vector to_vec(const std::string &s);
std::vector<std::string> split(const std::string &s,const std::string & delimiter);

void NotifyConnectionEstablishedUe(std::string context,
                                   uint64_t imsi,
                                   uint16_t cellid,
                                   uint16_t rnti);
void NotifyHandoverStartUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti,
                           uint16_t targetCellId);
void NotifyHandoverEndOkUe(std::string context,
                           uint64_t imsi,
                           uint16_t cellid,
                           uint16_t rnti);
void NotifyConnectionEstablishedEnb(std::string context,
                                    uint64_t imsi,
                                    uint16_t cellid,
                                    uint16_t rnti);
void NotifyHandoverStartEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti,
                            uint16_t targetCellId);
void NotifyHandoverEndOkEnb(std::string context,
                            uint64_t imsi,
                            uint16_t cellid,
                            uint16_t rnti);
void NotifyPacketSinkRx(std::string context, Ptr<const Packet> packet, const Address &address);
void ConnectTcpTrace(int nodeid);
void ReportProgress(Time reportingInterval);
void CongStateTrace(const TcpSocketState::TcpCongState_t oldValue, const TcpSocketState::TcpCongState_t newValue);                                                                                        
uint32_t ContextToNodeId (std::string context);
void ReportProgress(Time reportingInterval);


/* 
     ************************Function Declaration End********************************
*/

/* 
   ******************************** Configuration arguments Start********************************
*/
const std::string traceFilePrefix = "mmwave-tcp-x2-handover";
Time internetDelay=Time("0ms");
DataRate internetDataRate=DataRate("10Gb/s");
int internetMtu=1500;
Vector leftLtePos;
Vector rightLtePos;
Vector MNbPos1;
Vector MNbPos2;
Vector MNbPos3;
Vector MNbPos4;
Vector startPos;
Vector endPos;
Vector speed;
Time simTime;
std::string lteHandoverAlgorithm;
bool useIdealRrc;
bool rlcAmEnabled;
bool snrTest;
bool usePdschForCqiGeneration;
Time x2Latency;
Time mmeLatency;
int handoverMode;
int fixedTttValue;
int minDynTttValue;
int maxDynTttValue;
double minDiffTttValue;
double maxDiffValue;
int crtPeriod;
double outageThreshold;
int mmWaveEnb1TxPower;
int mmWaveEnb2TxPower;
int mmWaveEnb3TxPower;
int mmWaveEnb4TxPower;
int updateSinrEstimatePeriod;
double updateUeSinrEstimatePeriod;
bool noiseAndFilter;
std::string tcpType;


/* 
   ******************************** Configuration arguments End ********************************
*/

/* 
   ******************************* Output Stream start **************************************
*/
std::ofstream g_tcpCongStateTrace;
std::ofstream g_rttTrace;
std::ofstream g_bulkSendTx;
std::ofstream g_packetSinkRx;
std::ofstream g_cwndTrace;
/* 
   ******************************* Output Stream end **************************************
*/
