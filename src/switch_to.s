.section .text
.global switch_to
switch_to:
  call closealarm /* 模拟关中断 */
  push %rbp
  mov %rsp, %rbp /* 更改栈帧，以便寻参 */

  /* 保存现场 */
	push %rdi
	push %rsi
	push %rbx
	push %rdx
	push %rcx
	push %rax
  pushf
 
  /* 准备切换栈 */
  mov current, %rbx /* 取 current 基址放到 eax     */
  mov %rsp, 16(%rbx) /* 保存当前 esp 到线程结构体 */ 
  /* mov 16(%rbp), %rax /* 8(%ebp)即为c语言的传入参数next   取下一个线程结构体基址*/
  mov %rax, current /* 更新 current */
	mov 16(%rax), %rsp /* 切换到下一个线程的栈 */

  /* 恢复现场, 到这里，已经进入另一个线程环境了，本质是 esp 改变 */
  popf
	pop %rax
	pop %rcx
	pop %rdx
	pop %rbx
	pop %rsi
	pop %rdi
	pop %rbp
  call openalarm /* 模拟开中断  */
ret

