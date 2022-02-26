# Message Broker API Reference
Message Broker - TODO

---

## Protocol Adapters
### [Azure MQTT Protocol Adapter Libraries](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#azure-mqtt-protocol-adapter-libraries)

#### Install Additional dependencies
For an x86 computer running Ubuntu:
```
sudo apt-get install -y libcurl3 libssl-dev uuid-dev libglib2.0 libglib2.0-dev
```
For Jetson:
```
sudo apt-get install -y libcurl4-openssl-dev libssl-dev uuid-dev libglib2.0 libglib2.0-dev
```
#### Setup an Azure IoT Hub Instance
Follow the directions at https://docs.microsoft.com/en-us/azure/iot-hub/tutorial-connectivity#create-an-iot-hub.

#### Setup Broker Config File - specific to the protocol adpater library used

**For the Device-Client protocol adapter** - device-to-cloud messaging - use
* `/opt/nvidia/deepstream/deepstream/lib/libnvds_azure_proto.so` - and
* `/opt/nvidia/deepstream/deepstream/sources/libs/azure_protocol_adapter/device_client/ctg_azure` - as an example.

**For the Model-Client protocol adapter** - bidirectional device-remote-entity messaging - use
* `/opt/nvidia/deepstream/deepstream/lib/libnvds_azure_edge_proto.so` - and
* `/opt/nvidia/deepstream/deepstream/sources/libs/azure_protocol_adapter/module_client/ctg_azure` - as an example.

#### Setup set up Azure IoT Edge runtime on the edge device (require for Module-Client only)
For an x86 computer running Ubuntu:
 * Follow the instructions here. https://docs.microsoft.com/en-us/azure/iot-edge/how-to-install-iot-edge-linux

For Jetson:
* The best approach is still DBD.

### [AMQP Protocol Adapter](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#amqp-protocol-adapter)
Still to be tested.

### [REDIS Protocol Adapter](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#redis-protocol-adapter)
Still to be tested.

### [Kafka Protocol Adapter]()
Still to be tested.

--- 
## ODE Action API

**Callback Types:**
* [dsl_message_broker_connection_listener_cb](#dsl_message_broker_connection_listener_cb)
* [dsl_message_broker_send_result_listener_cb](#dsl_message_broker_send_result_listener_cb)
* [dsl_message_broker_subscriber_cb](#dsl_message_broker_subscriber_cb)

**Constructors:**
* [dsl_message_broker_new](#dsl_message_broker_new)

**Destructors:**
* [dsl_message_broker_delete](#dsl_message_broker_delete)
* [dsl_message_broker_delete_all](#dsl_message_broker_delete_all)

**Methods:**
* [dsl_message_broker_connect](#dsl_message_broker_connect)
* [dsl_message_broker_disconnect](#dsl_message_broker_disconnect)
* [dsl_message_broker_is_connected](#dsl_message_broker_is_connected)
* [dsl_message_broker_connection_listener_add](#dsl_message_broker_connection_listener_add)
* [dsl_message_broker_connection_listener_remove](#dsl_message_broker_connection_listener_remove)
* [dsl_message_broker_message_send_async](#dsl_message_broker_message_send_async)
* [dsl_message_broker_subscriber_add](#dsl_message_broker_subscriber_add)
* [dsl_message_broker_subscriber_remove](#dsl_message_broker_subscriber_remove)
* [dsl_message_broker_settings_get](#dsl_message_broker_settings_get)
* [dsl_message_broker_settings_set](#dsl_message_broker_settings_set)
* [dsl_message_broker_list_size](#dsl_message_broker_list_size)

## Constants
The following status values are used by the Message Broker API
```C
#define DSL_STATUS_BROKER_OK                                        0
#define DSL_STATUS_BROKER_ERROR                                     1
#define DSL_STATUS_BROKER_RECONNECTING                              2
#define DSL_STATUS_BROKER_NOT_SUPPORTED                             3
```

## Return Values
The following return codes are used by the Message Broker API
```C
#define DSL_RESULT_BROKER_RESULT                                    0x00800000
#define DSL_RESULT_BROKER_NAME_NOT_UNIQUE                           0x00800001
#define DSL_RESULT_BROKER_NAME_NOT_FOUND                            0x00800002
#define DSL_RESULT_BROKER_THREW_EXCEPTION                           0x00800003
#define DSL_RESULT_BROKER_IN_USE                                    0x00800004
#define DSL_RESULT_BROKER_SET_FAILED                                0x00800005
#define DSL_RESULT_BROKER_PARAMETER_INVALID                         0x00800006
#define DSL_RESULT_BROKER_SUBSCRIBER_ADD_FAILED                     0x00800007
#define DSL_RESULT_BROKER_SUBSCRIBER_REMOVE_FAILED                  0x00800008
#define DSL_RESULT_BROKER_LISTENER_ADD_FAILED                       0x00800009
#define DSL_RESULT_BROKER_LISTENER_REMOVE_FAILED                    0x0080000A
#define DSL_RESULT_BROKER_CONFIG_FILE_NOT_FOUND                     0x0080000B
#define DSL_RESULT_BROKER_PROTOCOL_LIB_NOT_FOUND                    0x0080000C
#define DSL_RESULT_BROKER_CONNECT_FAILED                            0x0080000D
#define DSL_RESULT_BROKER_DISCONNECT_FAILED                         0x0080000E
#define DSL_RESULT_BROKER_MESSAGE_SEND_FAILED                       0x0080000F
```

## Callback Types:
### *dsl_message_broker_connection_listener_cb*
```C
typedef void (*dsl_message_broker_connection_listener_cb)(void* client_data, uint status);
```
Callback typedef for a client to listen for connection events.

**Parameters**
* `client_data` [in]  opaque pointer to client's user data, provided by the client.  
* `status` [in] connection status. One of the [DSL_STATUS_BROKER](#constants) constants defined above.

<br>

### *dsl_message_broker_send_result_listener_cb*
```C
typedef void (*dsl_message_broker_send_result_listener_cb)(void* client_data, uint status);
```
Callback typedef for a client to listen for the asynchronous result when calling [dsl_message_broker_message_send_async](#dsl_message_broker_message_send_async).

**Parameters**
* `client_data` [in]  opaque pointer to client's user data, provided by the client.  
* `status` [in] result status. One of the [DSL_STATUS_BROKER](#constants) constants defined above.

<br>

### *dsl_message_broker_subscriber_cb*
```C
typedef void (*dsl_message_broker_subscriber_cb)(void* client_data, uint status,
    const wchar_t* topic, void *message, uint size);
```
Callback typedef for a client to listen for messages received from a remote entity.

**Parameters**
* `client_data` [in]  opaque pointer to the client's user data, provided by the client.  
* `status` [in] message status. One of the [DSL_STATUS_BROKER](#constants) constants defined above.
* `topic` [in] topic for the received message.
* `message` [in]  opaque pointer to the message received.
* `size` [in] size of the message in bytes.

<br>

---

## Constructors
### *dsl_message_broker_new*
```C++
DslReturnType dsl_message_broker_new(const wchar_t* name,
    const wchar_t* broker_config_file, const wchar_t* protocol_lib,
    const wchar_t* connection_string);
```
The constructor creates a uniquely named Message Broker. 

Refer to the [nvmsgbroker](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#) plugin documentation along with the README files located under the NVIDIA DeepStream installation folders `/opt/nvidia/deepstream/deepstream/sources/libs/<protocol-lib-adapter>` for information on Broker config settings and connection string syntax for each of the provided protocol adapter libraries.

**Parameters**
* `name` - [in] unique name for the Message Broker to create.
* `broker_config_file` - [in] absolute or relative path to a configuration file specific to the protocol-adapter used.
* `protocol_lib` - [in] absolute or relative path to the protocol-adapter library to use.
* `connection_string` - [in] (optional) full device connection string if not provided in the config file.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_new('my-message-broker', broker_config_file,
  protocol_lib, None')
```

<br>

---

## Destructors
### *dsl_message_broker_delete*
```C++
DslReturnType dsl_message_broker_delete(const wchar_t* name);
```
This destructor deletes a single named Message Broker. If connected, the broker will be disconnected from the remote entity before deletion.

**Parameters**
* `name` - [in] unique name for the Message Broker to delete.

**Returns**
* `DSL_RESULT_SUCCESS` on successful deletion. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_message_broker_delete('my-message-broker')
```

<br>

### *dsl_message_broker_delete_all*
```C++
DslReturnType dsl_message_broker_delete_all();
```
This destructor deletes all Message Brokers currently in memory. All connected Brokers will be disconnected from the entity before deletion.

**Returns**
* `DSL_RESULT_SUCCESS` on successful deletion. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_message_broker_delete_all()
```

<br>

---

## Methods

### *dsl_message_broker_connect*
```C++
DslReturnType dsl_message_broker_connect(const wchar_t* name);
```
This service attempts to connect the Message Broker to a remote entity. Clients can listen for connection events by add a callback of type type [dsl_message_broker_connection_listener_cb](#dsl_message_broker_connection_listener_cb) by calling [dsl_message_broker_connection_listener_add](#dsl_message_broker_connection_listener_add)

**Parameters**
* `name` - [in] unique name of the Message Broker to connect.

**Returns**
* `DSL_RESULT_SUCCESS` on successful connection. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_connect('my-message-broker')
```

<br>

### *dsl_message_broker_disconnect*
```C++
DslReturnType dsl_message_broker_disconnect(const wchar_t* name);
```
This service disconnects the Message Broker from a remote entity.

**Parameters**
* `name` - [in] unique name of the Message Broker to disconnect.

**Returns**
* `DSL_RESULT_SUCCESS` on successful disconnection. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_disconnect('my-message-broker')
```

<br>

### *dsl_message_broker_is_connected*
```C++
DslReturnType dsl_message_broker_is_connected(const wchar_t* name, boolean* connected);
```
This service returns the current connected state for the named Message Broker.

**Parameters**
* `name` - [in] unique name of the Message Broker to query.
* `connected` - [out] true if the Message Broker is connected, false otherwise.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, connected = dsl_message_broker_is_connected('my-message-broker')
```

<br>

### *dsl_message_broker_connection_listener_add*
```C++
DslReturnType dsl_message_broker_connection_listener_add(const wchar_t* name,
    dsl_message_broker_connection_listener_cb listener, void* client_data);
```
This service adds a callback function of type [dsl_message_broker_connection_listener_cb](#dsl_message_broker_connection_listener_cb) to a named Message Broker. Once added, the client will be called on each connection event that occurs.

**Parameters**
* `name` - [in] unique name of the Action to update.
* `listener` - [in] connection listener callback function to add.
* `client_data` - [in] opaque pointer to user data returned to the listener when the callback is called.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_connection_listener_add('my-message-broker', connection_listener, None)
```

<br>

### *dsl_message_broker_connection_listener_remove*
```C++
DslReturnType dsl_message_broker_connection_listener_remove(const wchar_t* name,
    dsl_message_broker_connection_listener_cb listener);
```
This service removes a callback function of type [dsl_message_broker_connection_listener_cb](#dsl_message_broker_connection_listener_cb) from a
named Message Broker, previously added with [dsl_message_broker_connection_listener_add](#dsl_message_broker_connection_listener_add)

**Parameters**
* `name` - [in] unique name of the Message Broker to update.
* `listener` - [in] connection listener callback function to remove.

**Returns**  
* `DSL_RESULT_SUCCESS` on successful removal. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_connection_listener_remove('my-message-broker', connection_listener)
```

<br>

### *dsl_message_broker_message_send_async*
```C++
DslReturnType dsl_message_broker_message_send_async(const wchar_t* name,
    const wchar_t* topic, void* message, size_t size,
    dsl_message_broker_send_result_listener_cb result_listener, void* user_data);
```
This service sends a message with an optional topic to a remote entity asynchronously. A callback function of type [dsl_message_broker_send_result_listener_cb](#dsl_message_broker_send_result_listener_cb) is used to signal the client with the asynchronous send result.

**Parameters**
* `name` - [in] unique name of the Message Broker to update.
* `topic` - [in] (optional) topic for the message.
* `message` - [in] opaque pointer to the message to send.
* `size` - [in] size of the message in bytes.
* `client_data` - [in] opaque pointer to user data returned to the listener when the callback is called.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python

retval = dsl_message_broker_message_send_async('my-message-broker',
  topic, message_body, result_listener, None)
```

<br>

### *dsl_message_broker_subscriber_add*
```C++
DslReturnType dsl_message_broker_subscriber_add(const wchar_t* name,
    dsl_message_broker_subscriber_cb subscriber, const wchar_t** topics,
    void* client_data);
```
This service adds a callback function of type [dsl_message_broker_subscriber_cb](#dsl_message_broker_subscriber_cb) to a named Message Broker. Once added, the client will be called with each message the Broker receives for one or more specified topics.

**Note:** Topics must be unique to each Subscriber.

**Parameters**
* `name` - [in] unique name of the Message Broker to update.
* `subscriber` - [in] message subscriber callback function to add.
* `topics` - [in] NULL terminated list of topics to subscribe to.
* `client_data` - [in] opaque pointer to user data returned to the listener when the callback is called.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
topics = ['/dsl/topic1', '/dsl/topic2', '/dsl/topic3', None]

retval = dsl_message_broker_connection_listener_add('my-message-broker',
  message_subscriber, topics, None)
```

<br>

### *dsl_message_broker_subscriber_remove*
```C++
DslReturnType dsl_message_broker_subscriber_remove(const wchar_t* name,
    dsl_message_broker_subscriber_cb subscriber);
```
This service removes a callback function of type [dsl_message_broker_subscriber_cb](#dsl_message_broker_subscriber_cb) from a
named Message Broker, previously added with [dsl_message_broker_subscriber_add](#dsl_message_broker_subscriber_add)

**Parameters**
* `name` - [in] unique name of the Message Broker to update.
* `listener` - [in] message subscriber callback function to remove.

**Returns**  
* `DSL_RESULT_SUCCESS` on successful removal. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_subscriber_remove('my-message-broker', message_subscriber)
```

<br>

### *dsl_message_broker_settings_get*
```C++
DslReturnType dsl_message_broker_settings_get(const wchar_t* name,
    const wchar_t** broker_config_file, const wchar_t** protocol_lib,
    const wchar_t** connection_string);
```
This service gets the current broker setttings in use for a named Message Broker.

Refer to the [nvmsgbroker](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#) plugin documentation along with the README files located under the NVIDIA DeepStream installation folders `/opt/nvidia/deepstream/deepstream/sources/libs/<protocol-lib-adapter>` for information on Broker config settings and connection string syntax for each of the provided protocol adapter libraries.

**Parameters**
* `name` - [out] unique name for the Message Broker to query.
* `broker_config_file` - [out] absolute path to the configuration file currently in use.
* `protocol_lib` - [out] absolute path to the protocol-adapter library in use.
* `connection_string` - [out] device connection string, an empty if not defined.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, broker_config_file, protocol_lib, connection_string = dsl_message_broker_settings_get('my-message-broker')
```

<br>

### *dsl_message_broker_settings_set*
```C++
DslReturnType dsl_message_broker_settings_set(const wchar_t* name,
    const wchar_t* broker_config_file, const wchar_t* protocol_lib,
    const wchar_t* connection_string);
```
This service sets new broker setttings for a named Message Broker to use. Note: this service will fail of the Message Broker is currently connected.

Refer to the [nvmsgbroker](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvmsgbroker.html#) plugin documentation along with the README files located under the NVIDIA DeepStream installation folders `/opt/nvidia/deepstream/deepstream/sources/libs/<protocol-lib-adapter>` for information on Broker config settings and connection string syntax for each of the provided protocol adapter libraries.

**Parameters**
* `name` - [in] unique name for the Message Broker to update.
* `broker_config_file` - [in] absolute or relative path to a configuration file specific to the protocol-adapter used.
* `protocol_lib` - [in] absolute or relative path to the protocol-adapter library to use.
* `connection_string` - [in] (optional) full device connection string if not provided in the config file.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_message_broker_settings_set('my-message-broker',
  broker_config_file, protocol_lib, None)
```

<br>

### *dsl_message_broker_list_size*
```c++
uint dsl_message_broker_list_size();
```
This service returns the size of the Message Broker container, i.e. the number of Message Brokers currently in memory.

**Returns**
* The size of the Message Broker container

**Python Example**
```Python
size = dsl_message_broker_list_size()
```

<br>

---

## API Reference
* [List of all Services](/docs/api-reference-list.md)
* [Pipeline](/docs/api-pipeline.md)
* [Player](/docs/api-player.md)
* [Source](/docs/api-source.md)
* [Tap](/docs/api-tap.md)
* [Dewarper](/docs/api-dewarper.md)
* [Inference Engine and Server](/docs/api-infer.md)
* [Tracker](/docs/api-tracker.md)
* [Segmentation Visualizer](/docs/api-segvisual.md)
* [Tiler](/docs/api-tiler.md)
* [Demuxer and Splitter](/docs/api-tee.md)
* [On-Screen Display](/docs/api-osd.md)
* [Sink](/docs/api-sink.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* [ODE Action](/docs/api-ode-action.md)
* [ODE Area](/docs/api-ode-area.md)
* [Display Type](/docs/api-display-type.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
* [Mailer](/docs/api-mailer.md)
* [WebSocket Server](/docs/api-ws-server.md)
* **Message Broker**