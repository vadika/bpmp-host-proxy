
#ifndef __BPMP__H__
#define __BPMP__H__

// hacks to adopt in-kernel types to userspace 
// #define __must_check 


// /* #include "linux/compiler_types.h"
// #include "linux/spinlock.h"
// // #include "linux/mailbox_client.h"
// // #include <sys/types.h>
// */

// #include <linux/spinlock_types.h>
// #include <stdbool.h>
// #include <pthread.h>
// #include <linux/semaphore.h>


// #include <linux/mailbox_client.h>
// #include <linux/pm_domain.h>
// #include <linux/reset-controller.h>
// #include <linux/semaphore.h>
// #include <sys/select.h>

///

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)



typedef unsigned char bool; //not sure, need to check!

typedef struct { } arch_spinlock_t;

typedef struct raw_spinlock {
	arch_spinlock_t raw_lock;
} raw_spinlock_t;


typedef struct spinlock {
	union {
		struct raw_spinlock rlock;
	};
} spinlock_t;


struct semaphore {
	raw_spinlock_t		lock;
	unsigned int		count;
	struct list_head	wait_list;
};



///

struct mbox_client {
	struct device *dev;
	bool tx_block;
	unsigned long tx_tout;
	bool knows_txdone;

	void (*rx_callback)(struct mbox_client *cl, void *mssg);
	void (*tx_prepare)(struct mbox_client *cl, void *mssg);
	void (*tx_done)(struct mbox_client *cl, void *mssg, int r);
};


struct tegra_bpmp {
	const struct tegra_bpmp_soc *soc;
	struct device *dev;
	void *priv;

	struct {
		struct mbox_client client;
		struct mbox_chan *channel;
	} mbox;

	spinlock_t atomic_tx_lock;
	struct tegra_bpmp_channel *tx_channel, *rx_channel, *threaded_channels;

	struct {
		unsigned long *allocated;
		unsigned long *busy;
		unsigned int count;
		struct semaphore lock;
	} threaded;

	struct list_head mrqs;
	spinlock_t lock;

	struct tegra_bpmp_clk **clocks;
	unsigned int num_clocks;

	struct reset_controller_dev rstc;

	struct genpd_onecell_data genpd;

};

struct tegra_bpmp_message {
	unsigned int mrq;

	struct {
		const void *data;
		size_t size;
	} tx;


	struct {
		void *data;
		size_t size;
		int ret;
	} rx;
};



#endif