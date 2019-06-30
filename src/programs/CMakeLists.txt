
add_executable(static_routing_simulation
  static_forwarding.cc
  # structure
  $<TARGET_OBJECTS:address>
  $<TARGET_OBJECTS:connection>
  $<TARGET_OBJECTS:event>
  $<TARGET_OBJECTS:interface>
  $<TARGET_OBJECTS:network>
  $<TARGET_OBJECTS:node>
  $<TARGET_OBJECTS:position>
  $<TARGET_OBJECTS:protocol_packet>
  $<TARGET_OBJECTS:routing>
  $<TARGET_OBJECTS:simulation>
  $<TARGET_OBJECTS:wireless_connection>
  # network_generator
  $<TARGET_OBJECTS:address_generator>
  $<TARGET_OBJECTS:event_generator>
  $<TARGET_OBJECTS:network_generator>
  $<TARGET_OBJECTS:position_generator>
  # static_routing
  $<TARGET_OBJECTS:static_routing>
)

add_executable(distance_vector_simulation
  distance_vector.cc
  # structure
  $<TARGET_OBJECTS:address>
  $<TARGET_OBJECTS:connection>
  $<TARGET_OBJECTS:event>
  $<TARGET_OBJECTS:interface>
  $<TARGET_OBJECTS:network>
  $<TARGET_OBJECTS:node>
  $<TARGET_OBJECTS:position>
  $<TARGET_OBJECTS:protocol_packet>
  $<TARGET_OBJECTS:routing>
  $<TARGET_OBJECTS:simulation>
  $<TARGET_OBJECTS:wireless_connection>
  # network_generator
  $<TARGET_OBJECTS:address_generator>
  $<TARGET_OBJECTS:event_generator>
  $<TARGET_OBJECTS:network_generator>
  $<TARGET_OBJECTS:position_generator>
  # distance_vector_routing
  $<TARGET_OBJECTS:dv_update_packet>
  $<TARGET_OBJECTS:dv_routing>
  # static_routing
  $<TARGET_OBJECTS:static_routing>
)

add_executable(sarp_simulation
  sarp.cc
  # structure
  $<TARGET_OBJECTS:address>
  $<TARGET_OBJECTS:connection>
  $<TARGET_OBJECTS:event>
  $<TARGET_OBJECTS:interface>
  $<TARGET_OBJECTS:network>
  $<TARGET_OBJECTS:node>
  $<TARGET_OBJECTS:position>
  $<TARGET_OBJECTS:protocol_packet>
  $<TARGET_OBJECTS:routing>
  $<TARGET_OBJECTS:simulation>
  $<TARGET_OBJECTS:wireless_connection>
  # network_generator
  $<TARGET_OBJECTS:address_generator>
  $<TARGET_OBJECTS:event_generator>
  $<TARGET_OBJECTS:network_generator>
  $<TARGET_OBJECTS:position_generator>
  # sarp
  $<TARGET_OBJECTS:sarp_address>
  $<TARGET_OBJECTS:sarp_routing_table>
  $<TARGET_OBJECTS:sarp_routing>
  $<TARGET_OBJECTS:sarp_update_packet>
  # static_routing
  $<TARGET_OBJECTS:static_routing>
)