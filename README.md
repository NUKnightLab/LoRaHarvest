# LoRaHarvest

Data collection library for use  with LoRa radio transmitters. For use with
the KnightLab SensorGrid project.

Communications utilize a tree-based collector (a.k.a. sink) node controlled
protocol which ensures that only a single node in the network is transmiting at
a given moment. This will not be the most efficient algorithm for data
collection but should prove to be more stable than other approaches we have seen.

## Principles

From our experimentation, network-carrier and receiver-timing contention among
nodes competing for communication time appear to be the biggest sources of
packet loss and missed communications in the relatively slow half-duplex
environment of consumer-available LoRa communications devices. All route
discovery, table-based routing protocols, and reliable (i.e. delivery-acknowledged)
protocols we have seen, have a significant level of contention occurring. Thus,
we are experimenting with an ACK-less non-routed protocol that utilizes
collector control for managing communications allowances.

Our primary driving principle with this (currently experimental) data
collection protocol is to enforce a mode of communication in which only a
single node is transmitting at a given moment. There is an exception to this
rule for certain types of control signals, however, generally our protocol will
work by communications control mechanisms enforced by the collector.

There are a set of knowns for our specific use cases which have become the
parameters within which we've developed this protocol. This is not a general
purpose communications protocol -- if your use case lies outside this set of
constraints, it might not be the best approach for your project.

 * **High latency is tolerable.** We are collecting environment data in mid to
   long-term deployments of solar powered devices. To conserve energy, our
   devices must regularly shift into a low-power mode during which no
   communications occur. The cycle of this mode is managed for all nodes by
   the collector device. The result is that a given data sample may not be
   collected for minutes, or even hours in some deployments. This is not a
   good choice of protocols for driving alarm conditions or real-time data
   collection.

 * **Nodes and node locations are enumerable and known.** The collector-control
   approach requires that the collector node is aware of all nodes in the
   network and knows the communications routes to those nodes. For simplicity
   and for ultimate reduction of network contention, no route-discovery is
   included in our protocol. This is not an appropriate protocol for ad-hoc
   unknown subscribers.

 * **Node locations are (relatively) stable.** This is implied by the previous
   point. While nodes can be moved and the network re-configured accordingly,
   some data loss can be expected during this transition. This is not the
   best protocol for mobile sensor devices.

Finally, we are setting out to create a communications protocol that is as
simple as possible. Protocol complexities and related code complexities have
led to numerous headaches in previous approaches. Our hope with this is that the
simplicity of the protocol leads not only to more stable communications, but
also results in simpler code that will be easier to test and to debug.

Our principles are summarized below:

 * stability/lowered-contention > short-term communications efficiency
 * long-term energy efficiency > low-latency
 * configuration > uncertainty
 * collector configuration > sensor-node configuration
