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
    input wire signed [17:0] angle_in,  // 2.16 fixed-point angle
    input wire start,
    output reg signed [17:0] cos_out,   // 2.16 fixed-point cosine
    output reg signed [17:0] sin_out,   // 2.16 fixed-point sine
    output reg done
);

    parameter ITERATIONS = 16;
    parameter signed [17:0] K = 18'd39849;  // ≈ 0.60725 * 65536

    // Arctangent lookup table (in 2.16 fixed-point)
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

    // Internal state
    reg [4:0] i;
    reg signed [17:0] x, y, z;
    reg signed [17:0] x_new, y_new;
    reg working;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            i <= 0;
            x <= 0;
            y <= 0;
            z <= 0;
            cos_out <= 0;
            sin_out <= 0;
            done <= 0;
            working <= 0;
        end else if (start && !working) begin
            // Initialize vector: x = K, y = 0, z = angle_in
            x <= K;
            y <= 0;
            z <= angle_in;
            i <= 0;
            working <= 1;
            done <= 0;
        end else if (working) begin
            if (i < ITERATIONS) begin
                if (z >= 0) begin
                    x_new = x - (y >>> i);
                    y_new = y + (x >>> i);
                    z = z - atan_table[i];
                end else begin
                    x_new = x + (y >>> i);
                    y_new = y - (x >>> i);
                    z = z + atan_table[i];
                end
                x <= x_new;
                y <= y_new;
                i <= i + 1;
            end else begin
                // Direct output (already scaled)
                cos_out <= x;
                sin_out <= y;
                done <= 1;
                working <= 0;
            end
        end
    end

endmodule
