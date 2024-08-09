package main
// #cgo LDFLAGS: -L"../../result/lib" -lemulator_wrapper
// #include "wrapper.hpp"
// #include <stdlib.h>
import "C"
import "unsafe"

import (
	"bufio"
	"context"
	"crypto/rand"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/signal"
	"syscall"
	"encoding/binary"

	"github.com/libp2p/go-libp2p"
	"github.com/libp2p/go-libp2p/core/crypto"
	"github.com/libp2p/go-libp2p/core/host"
	"github.com/libp2p/go-libp2p/core/network"
	"github.com/libp2p/go-libp2p/core/peer"
	"github.com/libp2p/go-libp2p/core/peerstore"

	"github.com/multiformats/go-multiaddr"
)

func handleStreamRead(s network.Stream) {
	log.Println("Got a new read stream!")

	// Create a buffer stream for non-blocking read and write.
	r := bufio.NewReader(bufio.NewReader(s))
	// Create EVM assigner runner instance
	runner := C.make_runner()

	go readData(r, runner)

	// stream 's' will stay open until you close it (or the other side closes it).
}

func readData(r *bufio.Reader, runner unsafe.Pointer) {
	for {
		var data [16]byte
		err := binary.Read(r, binary.BigEndian, &data)
		if err != nil {
			log.Println(err)
			return
		}

		if C.run(runner, *(*C.ulong)(&data[0]), *(*C.ulong(&data[8]))) != 0 {
			log.Println("run_single_thread FAILED")
			return
		}
		log.Println(data)
	}
}

func writeData(w *bufio.Writer) {
	var counter = 0
	config_file_name := C.CString("../assigner/example_data/call_block.json")
	for {
		// uint64_t shardId, uint64_t blockId
		var data [16]byte
		//TODO get IDs
		err := binary.Write(w, binary.BigEndian, data)
		if err != nil {
			log.Println(err)
			return
		}
		w.Flush()
		log.Printf("write %d blocks\n", counter)
		counter++;
	}
	C.free(unsafe.Pointer(config_file_name))
}

func main() {
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	dest := flag.String("d", "", "Destination multiaddr string")

	flag.Parse()

	// If debug is enabled, use a constant random source to generate the peer ID. Only useful for debugging,
	// off by default. Otherwise, it uses rand.Reader.
	var r io.Reader
	r = rand.Reader

	h, err := makeHost(0, r)
	if err != nil {
		log.Println(err)
		return
	}

	if *dest != "" {
		w, err := startPeerWrite(ctx, h, *dest)
		if err != nil {
			log.Println(err)
			return
		}
		// Create a thread to write data.
		go writeData(w)
	} else {
		startPeerRead(ctx, h, handleStreamRead)
	}

	// Wait Ctrl+C
	sigCh := make(chan os.Signal)
	signal.Notify(sigCh, syscall.SIGKILL, syscall.SIGINT)
	<-sigCh
}

func makeHost(port int, randomness io.Reader) (host.Host, error) {
	// Creates a new RSA key pair for this host.
	prvKey, _, err := crypto.GenerateKeyPairWithReader(crypto.RSA, 2048, randomness)
	if err != nil {
		log.Println(err)
		return nil, err
	}

	// 0.0.0.0 will listen on any interface device.
	sourceMultiAddr, _ := multiaddr.NewMultiaddr(fmt.Sprintf("/ip4/0.0.0.0/tcp/%d", port))

	// libp2p.New constructs a new libp2p Host.
	// Other options can be added here.
	return libp2p.New(
		libp2p.ListenAddrs(sourceMultiAddr),
		libp2p.Identity(prvKey),
	)
}

func startPeerRead(ctx context.Context, h host.Host, streamHandler network.StreamHandler) {
	// Set a function as stream handler.
	// This function is called when a peer connects, and starts a stream with this protocol.
	// Only applies on the receiving side.
	h.SetStreamHandler("/node/1.0.0", streamHandler)

	// Let's get the actual TCP port from our listen multiaddr, in case we're using 0 (default; random available port).
	var port string
	for _, la := range h.Network().ListenAddresses() {
		if p, err := la.ValueForProtocol(multiaddr.P_TCP); err == nil {
			port = p
			break
		}
	}

	if port == "" {
		log.Println("was not able to find actual local port")
		return
	}

	log.Printf("Run node %s", h.ID())
	log.Printf("Now start block generator on another console by command ./node -d /ip4/127.0.0.1/tcp/%v/p2p/%s'\n", port, h.ID())
}

func startPeerWrite(ctx context.Context, h host.Host, destination string) (*bufio.Writer, error) {
	// Turn the destination into a multiaddr.
	maddr, err := multiaddr.NewMultiaddr(destination)
	if err != nil {
		log.Println(err)
		return nil, err
	}

	// Extract the peer ID from the multiaddr.
	info, err := peer.AddrInfoFromP2pAddr(maddr)
	if err != nil {
		log.Println(err)
		return nil, err
	}

	// Add the destination's peer multiaddress in the peerstore.
	// This will be used during connection and stream creation by libp2p.
	h.Peerstore().AddAddrs(info.ID, info.Addrs, peerstore.PermanentAddrTTL)

	// Start a stream with the destination.
	// Multiaddress of the destination peer is fetched from the peerstore using 'peerId'.
	log.Printf("Try connect to %s", info.ID)
	s, err := h.NewStream(context.Background(), info.ID, "/node/1.0.0")
	if err != nil {
		log.Println(err)
		return nil, err
	}
	log.Println("Established connection to destination %s", info.ID)

	// Create a buffered stream so that read and writes are non-blocking.
	w := bufio.NewWriter(bufio.NewWriter(s))

	return w, nil
}
