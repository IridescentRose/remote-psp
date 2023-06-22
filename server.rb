require 'socket'

def local_ip
    Socket.ip_address_list.find { |addr| addr.ipv4_private? }.ip_address
end

server = TCPServer.new(local_ip, 3000)
puts "Server started at #{server.addr[3]}:#{server.addr[1]}"

module KeyID
    Select = 0x000001,
    Start = 0x000008,
    Up = 0x000010,
    Right = 0x000020,
    Down = 0x000040,
    Left = 0x000080,
    LTrigger = 0x000100,
    RTrigger = 0x000200,
    Triangle = 0x001000,
    Circle = 0x002000,
    Cross = 0x004000,
    Square = 0x008000,

    def self.match_key(key)
        case key
        when Select
          puts "Matched Select"
        when Start
          puts "Matched Start"
        when Up
          puts "Matched Up"
        when Right
          puts "Matched Right"
        when Down
          puts "Matched Down"
        when Left
          puts "Matched Left"
        when LTrigger
          puts "Matched LTrigger"
        when RTrigger
          puts "Matched RTrigger"
        when Triangle
          puts "Matched Triangle"
        when Circle
          puts "Matched Circle"
        when Cross
          puts "Matched Cross"
        when Square
          puts "Matched Square"
        else
          puts "No match found"
        end
      end    
end

# Let's outline the protocol
# Client connects 
# Sends a hello packet which gets returned
# Then server sends a streaming data
# And client sends events and regular updates
# Keepalive packet 1 per second

class Packet
    attr_accessor :id, :length, :data

    def initialize(id, length, data)
        @id = id
        @length = length
        @data = data
    end

end

def get_packet(client)
    id = client.read(1)
    return nil if id.nil? || id.empty?

    id_number = id.unpack('C').first

    length = client.read(4)
    return nil if length.nil? || length.empty?

    packet_length = length.unpack('N').first

    data = client.read(packet_length)
    return nil if data.nil? || data.empty?

    return Packet.new(id_number, packet_length, data)
end

def print_packet(packet)
    puts "Packet{id: #{packet.id}, length: #{packet.length}}"

    buffer = packet.data.unpack('C*')

    hex_string = "data: "
    buffer.each do |num|
        hex_string += "%02X" % num
    end

    puts hex_string
end

def handle_client(client)
    loop do

        readable = true
        loop do 
            readable = IO.select([client], nil, nil, 0.1)
            break if !readable

            packet = get_packet(client)
            break if packet.nil?
        
            if packet.id == 0x01
                key, press, hold = packet.data.unpack('Ncc')
                KeyID.match_key(key)
            end
        end
        
        break if client.closed?

        sleep 1.0 / 30.0
    end

    puts "Client disconnected."
end

def server_start(server)
    loop do 
        client = server.accept
        puts "Client connected to server at #{client.peeraddr[3]}:#{client.peeraddr[1]}"

        Thread.new(client) do |cl| 
            handle_client(cl)
        end
    end
end

begin 
    server_start(server)
rescue Interrupt
    print "\nServer stopped\n"
end