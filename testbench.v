// ==================================================================
// Testbench for CORDIC Rotation Mode Module (2.16 fixed-point)
//
// Provides test angles, triggers computation,
// and displays sine/cosine results in 2.16 format.
//
// Author: Seamus, Andre and Deniss
// ==================================================================

`timescale 1ns/1ps

module cordic_tb;

    reg clk = 0;
    reg rst = 0;
    reg start = 0;
    reg signed [17:0] angle_in;
    wire signed [17:0] cos_out, sin_out;
    wire done;

    cordic uut (
        .clk(clk),
        .rst(rst),
        .angle_in(angle_in),
        .start(start),
        .cos_out(cos_out),
        .sin_out(sin_out),
        .done(done)
    );

    always #5 clk = ~clk;

    // Helper task
    task run_test;
        input signed [17:0] angle;
        input [127:0] label;
        begin
            angle_in = angle;
            start = 1; #10 start = 0;
            wait (done);
            $display("%s => cos: %0d, sin: %0d", label, cos_out, sin_out);
            #20;
        end
    endtask

    initial begin
        $display("\n===== Clean CORDIC Test =====\n");

        rst = 1; #10 rst = 0;

        run_test(-18'sd102944, "-pi/2  (-1.5708)");
        run_test(-18'sd51471,  "-pi/4  (-0.7854)");
        run_test(-18'sd25736,  "-pi/8  (-0.3927)");
        run_test( 18'sd0,      "0      (0.0000)");
        run_test( 18'sd25736,  "pi/8   (0.3927)");
        run_test( 18'sd43690,  "pi/6   (0.5236)");
        run_test( 18'sd51471,  "pi/4   (0.7854)");
        run_test( 18'sd87381,  "pi/3   (1.0472)");
        run_test( 18'sd102944, "pi/2   (1.5708)");

        $display("\n===== Done =====");
        $finish;
    end

endmodule
