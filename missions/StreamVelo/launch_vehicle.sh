VNAME1="rex4USV"
VPORT1="9001"
VIP1="192.168.1.182"

SHARE_LISTEN1="9101"

MULTICAST1="multicast_10"


nsplug meta_vehicle.moos targ_$VNAME1.moos -f VNAME=$VNAME1 VPORT=$VPORT1 VIP=$VIP1 SHARE_LISTEN=$SHARE_LISTEN1 MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1 

pAntler targ_$VNAME1.moos >& /dev/null & uMAC targ_$VNAME1.moos
