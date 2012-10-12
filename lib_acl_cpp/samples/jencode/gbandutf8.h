/**
 * @file    gbandutf8_core.h
 * @author  Yang
 * @date    2004-06-18
 * @warning For MSWINDOWS and UNIX; MT_Level: MT-Safe.
 * @brief   �ڱ��ļ��ж�����һϵ�� GB2312 �� utf-8 �໥ת���ĺ���.
 */
/*-------------------------------------------------------------------------*/
#ifndef _GBANDUTF8_CORE_H
#define _GBANDUTF8_CORE_H
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"{
#endif
/*-------------------------------------------------------------------------*/
/**
 * utf-8 ת��Ϊ GB2312.
 * @param InBuf Ҫת���Ļ�����
 * @param InBufLen Ҫת���Ļ���������,�� �ֽ�(1 byte = 8 λ) Ϊ��λ
 * @param OutBuf ���ת����Ľ��
 * @param OutBufLen ����������ĳ���,�� �ֽ�(1 byte = 8 λ) Ϊ��λ
 * @return �ɹ���������������ĳ���,�� �ֽ�(1 byte = 8 λ) Ϊ��λ,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_Utf8ToGB2312(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * GB2312 ת��Ϊ utf-8.
 * @param InBuf Ҫת���Ļ����� 
 * @param InBufLen Ҫת���Ļ���������,�� �ֽ�(1 byte = 8 λ) Ϊ��λ
 * @param OutBuf ���ת����Ľ�� 
 * @param OutBufLen ����������ĳ���,�� �ֽ�(1 byte = 8 λ) Ϊ��λ
 * @return �ɹ���������������ĳ���,�� �ֽ�(1 byte = 8 λ) Ϊ��λ,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_GB2312ToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*/
#endif
/*-------------------------------------------------------------------------*/
