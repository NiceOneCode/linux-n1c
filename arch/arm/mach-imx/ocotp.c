/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/aes.h>
#include <linux/opt_ecdc.h>

static void __iomem *ocotp_base;

void __init imx_ocotp_init(const char *compat)
{
        struct device_node *ocotp_np;

        ocotp_np = of_find_compatible_node(NULL, NULL, compat);
        if (!ocotp_np) {
                pr_warn("failed to find ocotp node\n");
                return;
        }

        ocotp_base = of_iomap(ocotp_np, 0);
        if (!ocotp_base)
                pr_warn("failed to map ocotp\n");

        of_node_put(ocotp_np);
}

u32 imx_ocotp_read(u32 offset)
{
        if (WARN_ON(!ocotp_base))
                return 0;

        return readl_relaxed(ocotp_base + offset);
}

int imx_opotp_check (unsigned int addr_l,unsigned int addr_u)
{
        uint8_t addrs[17];
        unsigned int iicnt;
        struct AES_ctx ctx;
        Gdata addr;
        // printf("%08X%08X",addr_u,addr_l);
        sprintf((char *)(addrs),"%08X%08X",addr_u,addr_l);
        memcpy(addr.buffer,addrs,16);
        AES_init_ctx(&ctx, (unsigned char const *)(key));
        AES_ECB_encrypt(&ctx,(unsigned char const *)(addr.buffer));

        if(DIM_OPT_ECDC  <  4 )
        {
                return OPOTPOK;
        }

        for (iicnt=0; iicnt<DIM_OPT_ECDC; )
        {
                if(addr.UI.HH==OPT_ECDC[iicnt] )
                {
                        if(addr.UI.HL==OPT_ECDC[iicnt+1] )
                        {
                                if(addr.UI.LH==OPT_ECDC[iicnt+2])
                                {
                                        if(addr.UI.LL==OPT_ECDC[iicnt+3])
                                        {
                                                return OPOTPOK;
                                        }
                                }
                        }
                }
                iicnt+=4;
        }
        return OPOTPFAIL;
}
