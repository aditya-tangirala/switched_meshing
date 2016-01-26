#include<ns3/core-module.h>
#include<ns3/network-module.h>
#include<ns3/internet-module.h>
#include<ns3/point-to-point-module.h>
#include<ns3/applications-module.h>
#include<ns3/point-to-point-layout-module.h>
#include<iostream>
#include<sstream>
#include<iomanip>
#include<map>

using namespace ns3;


int main (int argc, char *argv[])
{
    uint32_t nLeaf=5;
    uint32_t maxPackets	=2000;
    uint32_t modeBytes=0;
    double minTh=1;
    double maxTh=10;
    uint32_t pktSize=8192;
    std::string	appDataRate="10Mbps";
    std::string	queueType="RED";
    uint16_t	port=5001;
    std::string	bottleNeckLinkBw="50Mbps";
    std::string	bottleNeckLinkDelay="50ms";

    CommandLine	cmd;
    cmd.AddValue	("nLeaf","Number of left and right side leaf nodes",	nLeaf);
    cmd.AddValue	("maxPackets","Max	Packets	allowed	in	the	queue",	maxPackets);
    cmd.AddValue	("queueType",	"Set Queue type to DropTail or RED",	queueType);
    cmd.AddValue	("appPktSize",	"Set OnOff App Packet Size",	pktSize);
    cmd.AddValue	("appDataRate",	"Set OnOff App DataRate",	appDataRate);
    cmd.AddValue	("modeBytes",	"Set Queue mode to Packets <0> or bytes <1>",	modeBytes);
    cmd.AddValue	("redMinTh",	"RED queue minimum threshold",	minTh);
    cmd.AddValue	("redMaxTh",	"RED queue maximum threshold",	maxTh);
    cmd.Parse	(argc,argv);


    if	((queueType!="RED")&&(queueType!="DropTail"))
    {
        NS_ABORT_MSG	("Invalid queue	type:Use --queueType=RED");
    }

    Config::SetDefault("ns3::OnOffApplication::PacketSize",	UintegerValue	(pktSize));
    Config::SetDefault("ns3::OnOffApplication::DataRate",	StringValue	(appDataRate));


    if	(!modeBytes)
    {
        Config::SetDefault("ns3::DropTailQueue::Mode",	StringValue	("QUEUE_MODE_PACKETS"));
        Config::SetDefault("ns3::DropTailQueue::MaxPackets",	UintegerValue	(maxPackets));
        Config::SetDefault("ns3::RedQueue::Mode",	StringValue	("QUEUE_MODE_PACKETS"));
        Config::SetDefault("ns3::RedQueue::QueueLimit",	UintegerValue	(maxPackets));
    }
    else
    {
        Config::SetDefault("ns3::DropTailQueue::Mode",	StringValue	("QUEUE_MODE_BYTES"));
        Config::SetDefault("ns3::DropTailQueue::MaxBytes",	UintegerValue	(maxPackets	*	pktSize));
        Config::SetDefault("ns3::RedQueue::Mode",	StringValue	("QUEUE_MODE_BYTES"));
        Config::SetDefault("ns3::RedQueue::QueueLimit",	UintegerValue	(maxPackets	*	pktSize));
        minTh *= pktSize;
        maxTh *= pktSize;
    }

    PointToPointHelper  bottleNeckLink;
    bottleNeckLink.SetDeviceAttribute   ("DataRate",    StringValue ("1000Mbps"));
    bottleNeckLink.SetChannelAttribute  ("Delay",   StringValue ("200ms"));

    if  (queueType=="RED")
    {
        bottleNeckLink.SetQueue ("ns3::RedQueue", 
                  "MinTh", DoubleValue (minTh),
                  "MaxTh", DoubleValue (maxTh));
    }
    PointToPointHelper  pointToPointLeaf;
    pointToPointLeaf.SetDeviceAttribute ("DataRate",    StringValue ("500Mbps"));
    pointToPointLeaf.SetChannelAttribute("Delay",   StringValue ("80ms"));

    PointToPointStarHelper	d (nLeaf, bottleNeckLink);

   

    InternetStackHelper	stack;
    d.InstallStack	(stack);
	 

    d.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"));  

    OnOffHelper clientHelper    ("ns3::TcpSocketFactory",   Address ());
    clientHelper.SetAttribute   ("OnTime",StringValue   ("ns3::UniformRandomVariable[Min=0.,Max=1.]"));
    clientHelper.SetAttribute   ("OffTime",StringValue  ("ns3::UniformRandomVariable[Min=0.,Max=1.]"));
    Address sinkLocalAddress(InetSocketAddress  (Ipv4Address::GetAny (), port));
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",  sinkLocalAddress);
    ApplicationContainer sinkApps;

     ApplicationContainer clientApps;

	 for (uint32_t j = 0; j < nLeaf; ++j)
        {
          	NodeContainer nodes;
  			nodes.Create(5);
  			NodeContainer n00= NodeContainer(d.GetSpokeNode(j),nodes.Get(0));
			NodeContainer n01= NodeContainer(d.GetSpokeNode(j),nodes.Get(1));
			NodeContainer n02= NodeContainer(d.GetSpokeNode(j),nodes.Get(2));
			NodeContainer n03= NodeContainer(d.GetSpokeNode(j),nodes.Get(3));
			NodeContainer n04= NodeContainer(d.GetSpokeNode(j),nodes.Get(4));

  			NetDeviceContainer d00=pointToPointLeaf.Install(n00);
  			NetDeviceContainer d01=pointToPointLeaf.Install(n01);
  			NetDeviceContainer d02=pointToPointLeaf.Install(n02);
  			NetDeviceContainer d03=pointToPointLeaf.Install(n03);
  			NetDeviceContainer d04=pointToPointLeaf.Install(n04);

  			InternetStackHelper outerstack;
  			outerstack.Install(nodes);
  			
            
            
			Ipv4AddressHelper ipv4;
			if(j==0)
			{ipv4.SetBase("10.2.1.0","255.255.255.0");
            ipv4.Assign (d00);
            ipv4.Assign (d01);
            ipv4.Assign (d02);
            ipv4.Assign (d03);
            ipv4.Assign (d04);			
            }
            else if(j==1)
			{ipv4.SetBase("10.3.1.0","255.255.255.0");
            ipv4.Assign (d00);
            ipv4.Assign (d01);
            ipv4.Assign (d02);
            ipv4.Assign (d03);
            ipv4.Assign (d04);
            }
            else if(j==2)
			{ipv4.SetBase("10.4.1.0","255.255.255.0");
            ipv4.Assign (d00);
            ipv4.Assign (d01);
            ipv4.Assign (d02);
            ipv4.Assign (d03);
            ipv4.Assign (d04);
            }
            else if(j==3)
			{ipv4.SetBase("10.5.1.0","255.255.255.0");
            ipv4.Assign (d00);
            ipv4.Assign (d01);
            ipv4.Assign (d02);
            ipv4.Assign (d03);
            ipv4.Assign (d04);
            }
            else if(j==4)
			{ipv4.SetBase("10.6.1.0","255.255.255.0");
            ipv4.Assign (d00);
            ipv4.Assign (d01);
            ipv4.Assign (d02);
            ipv4.Assign (d03);
            ipv4.Assign (d04);
            }

			
            
           
         
          
           for  (uint32_t i = 0;i < nLeaf; ++i)
            {   sinkApps.Add(packetSinkHelper.Install(nodes));
                AddressValue remoteAddress(InetSocketAddress(d.GetSpokeIpv4Address(i),port));
                clientHelper.SetAttribute("Remote",remoteAddress);
                clientApps.Add  (clientHelper.Install(nodes));
            }
		  
        } 

    
     
    
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(26.0));

   
    
    clientApps.Start(Seconds(1.0));	//	Start	1	second	after	sink
    clientApps.Stop(Seconds(8.0));	//	Stop	before	the	sink
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables	();

    std::cout<<"Running the simulation"	<<std::endl;
    Simulator::Run();
    uint32_t totalRxBytesCounter=0;

    for	(uint32_t i = 0; i < sinkApps.GetN();i++)
    {
        Ptr<Application> app = sinkApps.Get	(i);
        Ptr<PacketSink>	pktSink= DynamicCast <PacketSink>(app);
        totalRxBytesCounter += pktSink->GetTotalRx();
    }
    
                     Simulator::Destroy	();
                     return	0;




}