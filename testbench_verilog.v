`timescale 1ns/1ps

// ===========================================================
// Testbench for the CORDIC sine and cosine processor
// Simulates angles from -π/2 to +π/2 in 2.16 fixed-point
// format and compares results against expected math values.
// Also records waveforms for GTKWave viewing.
// ===========================================================

module cordic_tb;

    // -------------------------------
    // Inputs to the CORDIC module
    // -------------------------------
    reg clk = 0;                     // Clock signal
    reg rst = 0;                     // Reset signal
    reg start = 0;                   // Start signal to begin CORDIC operation
    reg signed [17:0] angle_in;      // Input angle in 2.16 fixed-point format (signed)

    // -------------------------------
    // Outputs from the CORDIC module
    // -------------------------------
    wire signed [17:0] cos_out;      // Output cosine value in 2.16 format
    wire signed [17:0] sin_out;      // Output sine value in 2.16 format
    wire done;                       // Signal that goes high when calculation is complete

    // Instantiate the CORDIC Unit Under Test (UUT)
    cordic uut (
        .clk(clk),
        .rst(rst),
        .angle_in(angle_in),
        .start(start),
        .cos_out(cos_out),
        .sin_out(sin_out),
        .done(done)
    );

    // Clock signal toggles every 5 ns (10 ns period)
    always #5 clk = ~clk;

    // -------------------------------
    // For comparison and evaluation
    // -------------------------------
    reg signed [17:0] expected_cos;  // Expected cosine value (computed using $cos())
    reg signed [17:0] expected_sin;  // Expected sine value (computed using $sin())
    reg [4:0] cos_match, sin_match;  // Number of leading matching bits

    // Function to count how many leading bits match
    function [4:0] count_leading_match;
        input [17:0] a, b;
        integer j;
        begin : compare_loop
            count_leading_match = 0;
            for (j = 17; j >= 0; j = j - 1) begin
                if (a[j] == b[j])
                    count_leading_match = count_leading_match + 1; // Bits match
                else
                    disable compare_loop; // Stop comparing once a bit differs
            end
        end
    endfunction

    // Real number representations for math comparison
    real rad;                // Real angle in radians
    real cos_real, sin_real; // Cosine and sine calculated with $cos() and $sin()

    // Variables for looping
    integer angle_fp;        // Angle in fixed-point to sweep through test cases
    integer step;            // Step size between test angles

    initial begin
        // -------------------------------
        // Waveform file generation setup
        // -------------------------------
        $dumpfile("cordic.vcd");       // File for GTKWave viewing
        $dumpvars(0, cordic_tb);       // Dump all signals in the testbench

        // Print table header to terminal
        $display("Angle (2.16) | Cos (Out) | Cos (Exp) | CosMatch | Sin (Out) | Sin (Exp) | SinMatch");

        // Apply reset at start
        rst = 1; #10 rst = 0;

        // Set step size for sweeping input angles (~1 degree steps)
        step = 1029;

        // Sweep input angles from -π/2 to +π/2
        for (angle_fp = -102944; angle_fp <= 102944; angle_fp = angle_fp + step) begin
            angle_in = angle_fp;  // Set input angle

            // Convert angle from fixed-point to real radians
            rad = angle_fp / 65536.0;

            // Compute expected values using built-in math functions
            cos_real = $cos(rad);
            sin_real = $sin(rad);

            // Convert expected values to fixed-point format
            expected_cos = $rtoi(cos_real * 65536.0);
            expected_sin = $rtoi(sin_real * 65536.0);

            // Trigger CORDIC calculation
            start = 1; #10 start = 0;  // Start pulse
            wait (done);              // Wait until done signal is high
            #5;                       // Delay to stabilize output

            // Compare outputs to expected and calculate matching bits
            cos_match = count_leading_match(cos_out, expected_cos);
            sin_match = count_leading_match(sin_out, expected_sin);

            // Print results to terminal
            $display("%d | %d | %d | %0d | %d | %d | %0d",
                     angle_in, cos_out, expected_cos, cos_match,
                     sin_out, expected_sin, sin_match);
        end

        $finish;  // End of simulation
    end

endmodule
