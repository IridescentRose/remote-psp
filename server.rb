require 'socket'

server = TCPServer.new('192.168.54.142', 3000)
print "Server started on port #{server.addr[1]}\n"


def server_start(server)
    loop do 
        client = server.accept
        print "Server received a client!\n"

        data = client.read(14)
        print "Received #{data}\n"

        client.puts "Hello, client!\n"

        client.close
    end
end

begin 
    server_start(server)
rescue Interrupt
    print "\nServer stopped\n"
end