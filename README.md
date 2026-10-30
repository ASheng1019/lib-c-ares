# c-ares Unikraft port

Build c-ares 1.34.9 from the verified upstream release, using normal
Config.uk/Makefile.uk rules. Public headers come from upstream; the musl ABI
configuration in include/ares_config.h is private. No DNS protocol source is
patched, and the port has no nghttpx or QUIC dependency.

The currently supported libc profile is musl. Select an Internet socket
provider (for example lwIP with its threaded socket API) separately. The port
selects the time, poll, pipe and getrandom facilities used by its configuration.
Supply a secure boot entropy source; do not use a fixed test seed in deployment.
IPv6 is not forced on: AF_INET6 declarations in musl are not a requirement for
an IPv6-capable network interface. Linux interface discovery and epoll discovery
are disabled in the private configuration; poll is available.

This replaces the old 1.14.0 source inventory and configuration. The old
LIBNEWLIBC selection refers to a removed libc path and is not restored. Other
libcs need their own ABI configuration and qualification before widening the
Kconfig dependency. A general-purpose port does not imply untested libc support.

The standalone tests/contract application exercises public resolver parsing,
channel configuration and event-thread lifecycles without nghttpx. The HTTP/3
workspace additionally tests real DNS success, loss, NXDOMAIN and address changes.
See tests/contract/defconfig for the independent library configuration.

The upstream MIT license is retained in COPYING.upstream. Existing port history
and authorship are preserved.

## Independent build and contract run

Set UK_ROOT, MUSL_PORT and PORT to separate core, libc and this port checkouts;
also set LWIP_PORT for the selected socket provider.
use fresh absolute OUT and CFG paths. No nghttpx repository is needed.

```sh
make -C "$UK_ROOT" A="$PORT/tests/contract" O="$OUT" C="$CFG" \
  L="$MUSL_PORT:$LWIP_PORT:$PORT" UK_DEFCONFIG="$PORT/tests/contract/defconfig" \
  COMPILER=clang-19 defconfig
make -C "$UK_ROOT" A="$PORT/tests/contract" O="$OUT" C="$CFG" \
  L="$MUSL_PORT:$LWIP_PORT:$PORT" COMPILER=clang-19 -j8
qemu-system-aarch64 -machine virt,gic-version=host -cpu host -accel kvm \
  -m 512M -smp 1 -display none -monitor none -serial stdio -no-reboot \
  -nic none -kernel "$OUT/contract_qemu-arm64"
```
