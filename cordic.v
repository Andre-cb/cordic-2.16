// ===========================================================
// CORDIC (Rotation Mode) - 2.16 Fixed-Point Implementation
// Computes sine and cosine of a given angle using only
// shifts and adds (no multipliers).
//
// Input: angle_in (2.16 fixed-point angle)
// Output: sin_out, cos_out (2.16 fixed-point results)
//
// Authors: Seamus, Andre and Deniss
// ===========================================================
module cordic (
    input wire clk,
    input wire rst,
    input wire signed [17:0] angle_in,  // Input angle in 2.16
    input wire start,
    output reg signed [17:0] cos_out,
    output reg signed [17:0] sin_out,
    output reg done
);

    parameter ITERATIONS = 16;
    parameter signed [17:0] K = 18'd39849; // ≈ 0.60725 * 65536

    // Arctangent lookup table (in 2.16 fixed-point format)
    reg signed [17:0] atan_table [0:15];
    initial begin
        atan_table[0]  = 18'd51471;
        atan_table[1]  = 18'd30385;
        atan_table[2]  = 18'd16055;
        atan_table[3]  = 18'd8148;
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

    // State variables
    reg [4:0] i;
    reg signed [17:0] x, y;
    reg signed [17:0] x_new, y_new;
    reg signed [17:0] A;               // Accumulated angle
    reg signed [17:0] theta;           // Target angle (latched)
    reg working;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            i <= 0;
            x <= 0;
            y <= 0;
            A <= 0;
            theta <= 0;
            cos_out <= 0;
            sin_out <= 0;
            done <= 0;
            working <= 0;
        end else if (start && !working) begin
            x <= K;
            y <= 0;
            A <= 0;
            theta <= angle_in;
            i <= 0;
            working <= 1;
            done <= 0;
            $display("=== CORDIC Start ===");
            $display("Input angle (fixed) = %0d", angle_in);
        end else if (working) begin
            if (i < ITERATIONS) begin
                //Check for undershoot or overshoot
                if (A <= theta) begin
                    // Undershoot: rotate counterclockwise (increase accumulated angle A)
                    x_new = x - (y >>> i);
                    y_new = y + (x >>> i);
                    A = A + atan_table[i];  // Add delta to accumulated angle A
                end else begin
                    // Overshoot: rotate clockwise (decrease accumulated angle A)
                    x_new = x + (y >>> i);
                    y_new = y - (x >>> i);
                    A = A - atan_table[i];  // Subtract delta from accumulated angle A
                end

                // Update values
                x <= x_new;
                y <= y_new;
                i <= i + 1;

                // Debug output for each iteration
                $display("iter=%0d | A=%0d | x=%0d | y=%0d", i, A, x_new, y_new);
            end else begin
                // Final result output
                cos_out <= x;
                sin_out <= y;
                done <= 1;
                working <= 0;
                $display("=== CORDIC Done ===");
                $display("Final Cos = %0d, Sin = %0d", x, y);
            end
        end
    end

endmodule
