cmd_/home/vadikas/BPMP/bpmp-host-proxy/modules.order := {   echo /home/vadikas/BPMP/bpmp-host-proxy/bpmp-host-proxy.ko; :; } | awk '!x[$$0]++' - > /home/vadikas/BPMP/bpmp-host-proxy/modules.order
