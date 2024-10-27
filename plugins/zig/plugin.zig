const std = @import("std");
const io = std.io;
const fs = std.fs;
const process = std.process;

var general_purpose_allocator: std.heap.GeneralPurposeAllocator(.{}) = .init;
const gpa = general_purpose_allocator.allocator();

export fn sum(x: i32, y: i32) i32 {
    return x + y;
}

export fn get_heap_allocated_string() [*c]const u8 {
    const str = "Greetings from the Zig plugin!";
    const heap_str = gpa.dupeZ(u8, str) catch return null;
    return heap_str.ptr;
}

export fn free_heap_allocated_string(heap_str: [*c]u8) void {
    var str: [:0]u8 = std.mem.span(heap_str);
    for (0..str.len) |i| {
        str[i] = 0;
    }
    gpa.free(str);
}

export fn test_print() void {
    io.getStdOut().writer().print("test_print(): Printing to stdout\n", .{}) catch return;
    io.getStdErr().writer().print("test_print(): Printing to stderr\n", .{}) catch return;
}

export fn test_file_io() void {
    var arena_instance = std.heap.ArenaAllocator.init(gpa);
    defer arena_instance.deinit();
    const arena = arena_instance.allocator();
    const preopens = fs.wasi.preopensAlloc(arena) catch return;

    std.debug.print("Preopens:\n", .{});
    for (preopens.names, 0..) |name, i| {
        std.debug.print("  {d}: {s} (strlen: {d})\n", .{i, name, name.len});
    }

    const path: [:0]const u8 = ".";
    const fd = preopens.find(path) orelse
        preopens.find(path[0..path.len+1]); // wasmer includes the null-terminator for some reason...
    if (fd == null) {
        std.debug.print("Failed to find filepath \".\"\n", .{});
        return;
    }

    const filename = "hello_zig.txt";
    const contents = "Hello from Zig!";

    // Write file
    {
        const dir = std.fs.Dir{ .fd = fd orelse unreachable };
        const file = dir.createFile(filename, .{}) catch return;
        _ = file.write(contents) catch return;
        file.close();
    }

    // Read file
    {
        const dir = std.fs.Dir{ .fd = fd orelse unreachable };
        const read_contents = dir.readFileAlloc(gpa,filename, contents.len) catch return;
        if (std.mem.eql(u8, contents, read_contents)) {
            std.debug.print("test_file_io(): File content is as expected\n", .{});
        }
        else {
            std.debug.print("test_file_io(): File content is not as expected\n", .{});
        }
    }
}

extern fn host_fn() i32;

export fn test_host_fn() void {
    io.getStdOut().writer().print("test_host_fn(): Got value: {d}", .{host_fn()}) catch return;
}
