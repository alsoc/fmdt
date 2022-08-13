/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#include <stdio.h>
#include <stdlib.h>

#include <CCL.h>
#include <assert.h>
#include <Ballon.h>

static uint32** g_er;   // Relative labels
static uint32** g_ea;   // Absolute labels
static uint32** g_era;  // Relative/Absolute labels equivalences;
static uint32** g_rlc;  // Run-length coding
static uint32*  g_eq;   // Table d'équivalence
static uint32*  g_ner;  // Number of relative labels

void CCL_LSL_init(int i0, int i1, int j0, int j1) {
    long n = (i1-i0+1)*(j1-j0+1);
    g_er  = ui32matrix(i0, i1, j0, j1);
    g_ea  = ui32matrix(i0, i1, j0, j1);
    g_era = ui32matrix(i0, i1, j0, j1);
    g_rlc = ui32matrix(i0, i1, j0, j1);
    g_eq  = ui32vector(0,  n);
    g_ner = ui32vector(i0, i1);
}

void CCL_LSL_free(int i0, int i1, int j0, int j1) {
    long n = (i1-i0+1)*(j1-j0+1);
    free_ui32matrix(g_er,  i0, i1, j0, j1);
    free_ui32matrix(g_ea,  i0, i1, j0, j1);
    free_ui32matrix(g_era, i0, i1, j0, j1);
    free_ui32matrix(g_rlc, i0, i1, j0, j1);
    free_ui32vector(g_eq,  0,  n);
    free_ui32vector(g_ner, i0, i1);
}

void LSL_Segment_Detection(uint32* line_er, uint32* line_rlc, uint32* line_ner, uint32* line, int j0, int j1) {
    uint32 j_curr;
    uint32 j_prev = 0;
    uint32 f = 0; // Front detection
    uint32 b = 0;
    uint32 er = 0;
    
    for (int j=j0 ; j<=j1 ; j++) {
        j_curr = line[j];
        f = j_curr ^ j_prev; // Xor: Front detection
        line_rlc[er] = j - (b & 1); // Begin/End of segment
        b ^= f; // Xor: End of segment correction
        er += (f & 1); // Increment label if front detected
        line_er[j] = er;
        j_prev = j_curr; // Save one load
    }
    j_curr = 0;
    f = j_curr ^ j_prev;
    line_rlc[er] = j1 + 1 - (b & 1);
    er += (f & 1);
    *line_ner = er;
}

void LSL_Equivalence_Construction(uint32* line_rlc, uint32* line_era, uint32* prevline_er, uint32* prevline_era, int n, int x0, int x1, uint32* nea) {
    int k, er, j0, j1, er0, er1, ea, a, erk, eak, ak;
    for (k=0 ; k<n ; k+=2) {
        er = k+1;
        
        j0 = line_rlc[k]; // Segment begin
        j1 = line_rlc[er]; // Segment end (k+1)
        
        // Extends for 8-connected
        if (j0 > x0)
            j0 -= 1;
        if (j1 < x1)
            j1 += 1;
        
        er0 = prevline_er[j0]; 
        er1 = prevline_er[j1];
        
        if ((er0 & 1) == 0) // er0 is even
            er0 += 1;
        if ((er1 & 1) == 0) // er1 is even
            er1 -= 1;
            
        if (er1 >= er0) { // Adjacency -> connect components
            ea = prevline_era[er0];
            a = g_eq[ea];
            for (erk=er0+2 ; erk<=er1 ; erk+=2) {
                eak = prevline_era[erk];
                ak = g_eq[eak];
                while (ak != g_eq[ak]) {ak = g_eq[ak];}
                if (a < ak) {
                    g_eq[eak] = a; // Minimum propagation
                }
                
                if (a > ak) {
                    a = ak;
                    g_eq[ea] = a;
                    ea = eak;
                }
            }
            line_era[er] = a; // Global minimum
        } else { // No adjacency -> new label
            line_era[er] = *nea;
            g_eq[*nea] = *nea;
            (*nea)++;
        }
    }
}
// ---------------------------------------------------------
uint32 CCL_LSL(uint32** img, int i0, int i1, int j0, int j1)
// ---------------------------------------------------------
 {
    // Step #1 - Segment detection
    for (int i=i0 ; i<=i1 ; i++) {
        LSL_Segment_Detection(g_er[i], g_rlc[i], &g_ner[i], img[i], j0, j1);
    }
    
    // Step #2 - Equivalence construction
    uint32 nea = i0;
    uint32 n = g_ner[i0];
    for (int k=0 ; k<n ; k+=2) {
        g_eq[nea] = nea;
        g_era[i0][k+1] = nea++;
    } 
    for (int i=i0+1 ; i<=i1 ; i++) {
        LSL_Equivalence_Construction(g_rlc[i], g_era[i], g_er[i-1], g_era[i-1], g_ner[i], j0, j1, &nea);
    }
    
    // Step #3 - Relative to Absolute label conversion
    
    // Step #4 - Resolution of equivalence classes
    uint32 trueN = 0;
    for (int i=0 ; i<nea ; i++) {
        if (i != g_eq[i]) {
            g_eq[i] = g_eq[g_eq[i]];
        } else {
            g_eq[i] = trueN++;
        }
    }
    
    // Step #5 - Final image labeling
    for (int i=i0 ; i<=i1 ; i++) {
        n = g_ner[i];
        for (int k=0 ; k<n ; k+=2) {
            int a = g_rlc[i][k];
            int b = g_rlc[i][k+1];
            
            // Step #3 merged with step #5
            uint32 val = g_era[i][g_er[i][a]];
            val = g_eq[val]+1;
            
            for (int j=a ; j<=b ; j++)
                img[i][j] = val;
        }
    }

    assert(trueN < SIZE_MAX_METEORROI);

    return trueN;
}
