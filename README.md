# bpmp-host-proxy


to compile -- patch the Linux_for_Tegra/source/public/kernel/kernel-5.10/drivers/firmware/tegra/bpmp.c
```
diff -p1 bpmp.c.orig bpmp.c
*** bpmp.c.orig 2023-03-28 09:42:46.854603014 +0300
--- bpmp.c      2023-03-28 09:44:52.101533703 +0300
*************** channel_to_ops(struct tegra_bpmp_channel
*** 42,43 ****
--- 42,45 ----
  
+ struct tegra_bpmp *tegra_bpmp_host_device=NULL;
+ 
  struct tegra_bpmp *tegra_bpmp_get(struct device *dev)
*************** put:
*** 67,71 ****
--- 69,76 ----
        of_node_put(np);
+       tegra_bpmp_host_device=bpmp;
        return bpmp;
  }
+ 
  EXPORT_SYMBOL_GPL(tegra_bpmp_get);
+ EXPORT_SYMBOL_GPL(tegra_bpmp_host_device);
```
