# dbus_template

做一个通用的基于json的dbus通信模板。后续有需要，都可以基于这个来做。

```
myservice.xml
	描述通信内容格式。
myservice_client.c 
	一个一直循环的client。
	可以验证Notification的接收处理。
myservice_cmd.c  
	一个命令行工具，作为client发送命令进行测试
myservice_server.c  
	dbus的服务端。接收get/set，发出Notification。
```

# 修改记录

现在把server和cmd写了。

执行cmd的时候，报错：

```
Cannot autolaunch D-Bus without X11 $DISPLAY.
```

先执行下面的语句，则可以解决这个问题。

```
for i in `/usr/bin/dbus-launch | /bin/sed '/^#.*\|^$/d'`; do
export $i
done
mkdir -p /tmp/dbus
echo DBUS_SESSION_BUS_ADDRESS=$DBUS_SESSION_BUS_ADDRESS > /tmp/dbus/dbus-addr
echo DBUS_SESSION_BUS_PID=$DBUS_SESSION_BUS_PID > /tmp/dbus/dbus-pid
```

