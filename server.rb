require 'socket'

server = TCPServer.new('localhost', 6969)
print "Server started on port #{server.addr[1]}\n"


def server_start(server)
    loop do 
        client = server.accept

        request = client.gets
        puts "Received #{request}"

        client.puts "Hello, client!"

        client.close
    end
end

begin 
    server_start(server)
rescue Interrupt
    puts "\nServer stopped"
end