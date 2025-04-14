`timescale 1ns/1ps

// ===========================================================
// CORDIC (Rotation Mode) - 2.16 Fixed-Point Implementation
// Computes cosine and sine using iterative shifts and adds
// ===========================================================

module cordic (
    input wire clk,                           // Clock
    input wire rst,                           // Reset
    input wire signed [17:0] angle_in,        // Input angle in 2.16 format (radians)
    input wire start,                         // Start signal to begin computation
    output reg signed [17:0] cos_out,         // Output cosine value (2.16 fixed-point)
    output reg signed [17:0] sin_out,         // Output sine value (2.16 fixed-point)
    output reg done                           // Done flag to indicate completion
);

    // Number of CORDIC iterations (precision limited to 14)
    parameter ITERATIONS = 14;

    // Scaling constant (K ≈ 0.60725 * 65536 ≈ 39849 in 2.16 fixed-point)
    parameter signed [17:0] K = 18'd39849;

    // Lookup table of arctangent values in 2.16 fixed-point
    reg signed [17:0] atan_table [0:15];
    initial begin
        atan_table[0]  = 18'd51471;  // atan(2^0)   ≈ 45°
        atan_table[1]  = 18'd30385;  // atan(2^-1)  ≈ 26.565°
        atan_table[2]  = 18'd16054;  // atan(2^-2)
        atan_table[3]  = 18'd8145;
        atan_table[4]  = 18'd4090;
        atan_table[5]  = 18'd2045;
        atan_table[6]  = 18'd1023;
        atan_table[7]  = 18'd512;
        atan_table[8]  = 18'd256;
        atan_table[9]  = 18'd128;
        atan_table[10] = 18'd64;
        atan_table[11] = 18'd32;
        atan_table[12] = 18'd16;
        atan_table[13] = 18'd8;
        atan_table[14] = 18'd4;
        atan_table[15] = 18'd2;
    end

    // Internal registers
    reg signed [17:0] x, y, z;    // x = cos, y = sin, z = angle accumulator
    reg [4:0] i;                  // Iteration counter (5 bits for up to 32)
    reg running;                 // Active flag

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            // Reset all values
            cos_out <= 0;
            sin_out <= 0;
            done <= 0;
            running <= 0;
        end else begin
            if (start && !running) begin
                // Begin CORDIC operation
                x <= K;          // Start with scaling factor
                y <= 0;          // y starts at 0
                z <= angle_in;   // Input angle loaded into accumulator
                i <= 0;          // Reset iteration counter
                running <= 1;    // Mark as running
                done <= 0;
                $display("=== CORDIC Start ===");
                $display("Input angle (fixed) = %d", angle_in);
            end else if (running) begin
                // Perform one iteration of CORDIC
                if (i < ITERATIONS) begin
                    if (z >= 0) begin
                        // Rotate clockwise
                        x <= x - (y >>> i);
                        y <= y + (x >>> i);
                        z <= z - atan_table[i];
                    end else begin
                        // Rotate counterclockwise
                        x <= x + (y >>> i);
                        y <= y - (x >>> i);
                        z <= z + atan_table[i];
                    end

                    // Debug output per iteration
                    $display("iter=%0d | A=%0d | x=%0d | y=%0d", i, z, x, y);
                    i <= i + 1;
                end else begin
                    // All iterations done, output result
                    cos_out <= x;
                    sin_out <= y;
                    done <= 1;
                    running <= 0;
                    $display("=== CORDIC Done ===");
                    $display("Final Cos = %d, Sin = %d", x, y);
                end
            end
        end
    end

endmodule
