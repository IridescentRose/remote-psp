require 'socket'

server = TCPServer.new('127.0.0.1', 3000)
print "Server started on port #{server.addr[1]}\n"

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

def handle_client(client)
    loop do
        packet = get_packet(client)
        break if packet.nil?

        buffer = packet.data.unpack('C*')

        puts "Packet{id: #{packet.id}, length: #{packet.length}}"

        hex_string = "data: "
        buffer.each do |num|
            # Option 2: Using %x format specifier
            hex_string += "%02X" % num
        end

        puts hex_string
    end

    puts "Client disconnected."
end

def server_start(server)
    loop do 
        client = server.accept
        print "Server received a client!\n"

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