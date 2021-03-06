 /*
 *==============================================================================
 *
 * Copyright (C) 2018 Joshua Vega (@jsvcycling)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *------------------------------------------------------------------------------
 *
 * An implementation of the Hodkin-Huxley model in C using floats.
 *
 *==============================================================================
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

/* Using preprocessor statements to reduce memory calls. */
#define C_m 1.f

#define g_K   36.f
#define g_Na  120.f
#define g_L   0.3f

#define V_K   -12.f
#define V_Na  115.f
#define V_L   10.6f

#define t_max   10000.f
#define dt      0.01f

#define I_start_time  1000.f
#define I_end_time    5000.f
#define I             12.f

float alpha_n(float v) {
  return 0.01f*(10.f - v) / (expf((10.f - v)/10.f) - 1.f);
}

float alpha_m(float v) {
  return 0.1f*(25.f - v) / (expf((25.f - v)/10.f) - 1.f);
}

float alpha_h(float v) {
  return 0.07f * expf(-v / 20.f);
}

float beta_n(float v) {
  return 0.125f * expf(-v / 80.f);
}

float beta_m(float v) {
  return 4.f * expf(-v / 18.f);
}

float beta_h(float v) {
  return 1.f / (expf((30.f - v)/10.f) + 1.f);
}

float heaviside(float x) {
  if (x >= 1.f) return 1.f;
  else return 0.f;
}

int main(int argc, char **argv) {
  int num_ts = (int)ceilf(t_max / dt);

  vector<float> t(num_ts);
  vector<float> V(num_ts);
  vector<float> N(num_ts);
  vector<float> M(num_ts);
  vector<float> H(num_ts);

  t[0] = 0.f;
  
  for (int i = 1; i < num_ts; i++) {
    t[i] = t[i-1] + dt;
  }

  V[0] = V_L;
  N[0] = alpha_n(V[0]);
  M[0] = alpha_m(V[0]);
  H[0] = alpha_h(V[0]);

  for (int i = 0; i < num_ts - 1; i++) {    
    float Iapp = I*heaviside(t[i] - I_start_time)*heaviside(I_end_time - t[i]);

    float I_K1 = g_K*powf(N[i], 4)*(V[i] - V_K);
    float I_Na1 = g_Na*powf(M[i], 3)*H[i]*(V[i] - V_Na);
    float I_L1 = g_L*(V[i] - V_L);

    float V_1 = (Iapp - I_K1 - I_Na1 - I_L1)/C_m;
    float N_1 = alpha_n(V[i])*(1.f - N[i]) - beta_n(V[i])*N[i];
    float M_1 = alpha_m(V[i])*(1.f - M[i]) - beta_m(V[i])*M[i];
    float H_1 = alpha_h(V[i])*(1.f - H[i]) - beta_h(V[i])*H[i];

    float aV = V[i] + V_1*dt;
    float aN = N[i] + N_1*dt;
    float aM = M[i] + M_1*dt;
    float aH = H[i] + H_1*dt;

    float I_K2 = g_K*powf(aN, 4)*(aV - V_K);
    float I_Na2 = g_Na*powf(aM, 3)*aH*(aV - V_Na);
    float I_L2 = g_L*(aV - V_L);

    float V_2 = (Iapp - I_K2 - I_Na2 - I_L2)/C_m;
    float N_2 = alpha_n(aV)*(1.f - aN) - beta_n(aV)*aN;
    float M_2 = alpha_m(aV)*(1.f - aM) - beta_m(aV)*aM;
    float H_2 = alpha_h(aV)*(1.f - aH) - beta_h(aV)*aH;

    V[i+1] = V[i] + (V_1 + V_2) * dt / 2.f;
    N[i+1] = N[i] + (N_1 + N_2) * dt / 2.f;
    M[i+1] = M[i] + (M_1 + M_2) * dt / 2.f;
    H[i+1] = H[i] + (H_1 + H_2) * dt / 2.f;
  }

  cout << V[num_ts - 1] << endl;

  exit(EXIT_SUCCESS);
}
