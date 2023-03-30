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



extended version of the patch to dump the datastructures also is below
```
*** bpmp.c.orig 2023-03-28 09:42:46.854603014 +0300
--- bpmp.c      2023-03-30 13:58:44.686832600 +0300
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
  
*************** int tegra_bpmp_transfer(struct tegra_bpm
*** 370,371 ****
--- 375,380 ----
                                            msg->tx.size);
+ 
+       printk("tegra_bpmp_transfer tx: %x tx.size= %ld \n", msg->mrq, msg->tx.size);
+       print_hex_dump(KERN_INFO, "tegra_bpmp_transfer tx:" ,DUMP_PREFIX_NONE, 16, 1, msg->tx.data, msg->tx.size, false);
+ 
        if (IS_ERR(channel))
*************** int tegra_bpmp_transfer(struct tegra_bpm
*** 373,375 ****
  
!       err = tegra_bpmp_ring_doorbell(bpmp);
        if (err < 0)
--- 382,384 ----
  
!       err = tegra_bpmp_ring_doorbell(bpmp);
        if (err < 0)
*************** int tegra_bpmp_transfer(struct tegra_bpm
*** 383,386 ****
  
!       return tegra_bpmp_channel_read(channel, msg->rx.data, msg->rx.size,
                                       &msg->rx.ret);
  }
--- 392,401 ----
  
! 
!       err= tegra_bpmp_channel_read(channel, msg->rx.data, msg->rx.size,
                                       &msg->rx.ret);
+ 
+       printk("tegra_bpmp_transfer rx: err=%d\n msg->rx.ret=%d", err, msg->rx.ret);
+       print_hex_dump(KERN_INFO,"tegra_bpmp_transfer rx:" ,DUMP_PREFIX_NONE, 16, 1, msg->rx.data, msg->rx.size, false);
+ 
+       return err;
  }
```
