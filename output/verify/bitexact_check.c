/* Bitwise no-regression: checked entry vs original API over random VALID
 * inputs. Every destination float must compare equal via memcmp. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include <cglm/validate.h>

static float frand(void) { return (float)((rand() % 2001) - 1000) / 37.0f; }

int main(void) {
  srand(42);
  int mismatches = 0, n = 20000;

  for (int t = 0; t < n; t++) {
    int kind = t % 4;
    if (kind <= 2) {                 /* vector normalize, sizes 2/3/4 */
      int dim = kind + 2;
      float a[4], b[4], rc[4], rp[4];
      for (int i = 0; i < dim; i++) { a[i] = b[i] = frand() + 1.0f; }
      glm_validate_status_t st;
      if (dim == 2) st = glm_validate_vec2_normalize_to(a, rc), glm_vec2_normalize_to(b, rp);
      else if (dim == 3) st = glm_validate_vec3_normalize_to(a, rc), glm_vec3_normalize_to(b, rp);
      else st = glm_validate_vec4_normalize_to(a, rc), glm_vec4_normalize_to(b, rp);
      if (st != GLM_VALIDATE_OK || memcmp(rc, rp, (size_t)dim * sizeof(float)) != 0)
        mismatches++;
    } else {                         /* mat4 inverse */
      mat4 m, mc, rc, rp;
      do {
        for (int i = 0; i < 16; i++) ((float *)m)[i] = frand();
        glm_mat4_copy(m, mc);
      } while (fabsf(glm_mat4_det(m)) < 1.0f);   /* keep only invertible ones */
      glm_validate_status_t st = glm_validate_mat4_inv(m, rc);
      glm_mat4_inv(mc, rp);
      if (st != GLM_VALIDATE_OK || memcmp(rc, rp, sizeof(mat4)) != 0)
        mismatches++;
    }
  }
  printf("%d valid-input samples, %d bitwise mismatches\n", n, mismatches);
  return mismatches != 0;
}
