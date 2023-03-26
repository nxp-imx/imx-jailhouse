Run zephyr demo on i.MX8MM
============================

Build zephyr demo on i.MX8MM
============================

```
The tested commit is: 523de0d9be8020b305d86c7b3cabf07c5342841d
i.MX8MM:
west build -p auto --board=mimx8mm_evk_a53 samples/synchronization
i.MX8MP:
west build -p auto --board=mimx8mp_evk_a53 samples/synchronization
```

```bash
 In U-Boot state:
 run 'jh_mmcboot' to start.

 After root cell ready:

 ./jailhouse enable imx8mm.cell
 ./jailhouse cell create imx8mm-zephyr.cell
 ./jailhouse cell load 1 zephyr.bin -a 0x93c00000
 ./jailhouse cell start 1

 ./jailhouse enable imx8mp.cell
 ./jailhouse cell create imx8mp-zephyr.cell
 ./jailhouse cell load 1 zephyr.bin -a 0xc0000000
 ./jailhouse cell start 1

```
