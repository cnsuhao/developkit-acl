/**
 * @file    utf8andunicode_core.h
 * @author  Yang
 * @date    2003-04-15
 * @warning For MSWINDOWS and UNIX; MT_Level: MT-Safe.
 * @brief   �ڱ��ļ��ж�����һϵ�к� utf8 �� unicode �໥ת���ĺ���,�˴���ָ UNICODELITTLE.
 */
/*-------------------------------------------------------------------------*/
#ifndef _UTF8ANDUNICODE_CORE_H
#define _UTF8ANDUNICODE_CORE_H
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"{
#endif
/*-------------------------------------------------------------------------*/
/**
 * �� utf-8 ת��Ϊ ucs-2.
 * @param InBuf ��ת�������뻺����
 * @param InBufLen ���뻺������Ҫת�����ֽ���(��: byte ��,1 byte = 8 λ)
 * @param OutBuf ����Ļ�����
 * @param OutBufLen ����������Ĵ�С,�� �ֽ�(1 byte = 8 λ)Ϊ��λ
 * @return �ɹ����� OutBuf ���� ucs-2 Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_Utf8ToUCS2(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * �� ucs-2 ת��Ϊ utf-8.
 * @param InBuf ��ת�������뻺����
 * @param InBufLen ���뻺������Ҫת�����ֽ���(��: byte ��,1 byte = 8 λ)
 * @param OutBuf ����Ļ�����
 * @param OutBufLen ����������Ĵ�С,���ֽ�(1 byte = 8 λ)Ϊ��λ
 * @return �ɹ����� OutBuf ���� �ֽ�(1 byte = 8 λ) Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_UCS2ToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * �� utf-8 ת��Ϊ unicode.
 * @param InBuf ��ת�������뻺����
 * @param InBufLen ���뻺������Ҫת�����ֽ���(��: byte ��,1 byte = 8 λ)
 * @param OutBuf ����Ļ�����
 * @param OutBufLen ����������Ĵ�С,�� �ֽ�(1 byte = 8 λ)Ϊ��λ
 * @return �ɹ����� OutBuf ���� unicode(ucs-2) Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���<BR>
 * ��������ָ�� unicode Ϊ ucs-2</P>
 */
int LC_Utf8ToUnicode(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * �� unicode ת��Ϊ utf-8.
 * @param InBuf ��ת�������뻺����
 * @param InBufLen ���뻺������Ҫת�����ֽ���(��: byte ��,1 byte = 8 λ)
 * @param OutBuf ����Ļ�����
 * @param OutBufLen ����������Ĵ�С,�� �ֽ�(1 byte = 8 λ)Ϊ��λ
 * @return �ɹ����� OutBuf ���� �ֽ�(1 byte = 8 λ) Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>InBuf ����Ϊ NULL,InBufLen ����Ϊ 0,����᷵�ش���<BR>
 * OutBuf ����Ϊ NULL,OutBufLen ����Ϊ 0,����᷵�ش���<BR>
 * InBuf �� OutBuf ָ��Ļ����������ص�,����᷵�ش���<BR>
 * ��������ָ�� unicode Ϊ ucs-2</P>
 */
int LC_UnicodeToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);
/*-------------------------------------------------------------------------*/

/**
 * �ڲ�����,������ֱ��ʹ��.
 * �� ucs-2 ת��Ϊ ���ֽ��ַ���.
 * @param WideChar ��ת���������ַ���
 * @param WideCharLen �����ַ������� unicode(ucs-2) Ϊ��λ���ַ�����
 * @param CharBuf ����Ļ�����
 * @param CharBufLen ����������Ĵ�С,�� �ֽ�(1 byte = 8 λ)Ϊ��λ
 * @param CodeSet �������ʹ�õ��ַ���.
 * �� CodeSet == NULL ʱʹ�� GB2312,�����ĵ��ֽ��ַ�Ϊ GB2312
 * @return �ɹ����� CharBuf ���� �ֽ�(1 byte = 8 λ) Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>WideChar ����Ϊ NULL,WideCharLen ����Ϊ 0,����᷵�ش���<BR>
 * CharBuf ����Ϊ NULL,CharBufLen ����Ϊ 0,����᷵�ش���<BR>
 * WideChar �� CharBuf ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_CharFromWideChar2(const void * WideChar, const unsigned int WideCharLen,
		void * CharBuf, const unsigned int CharBufLen,
		const char * CodeSet);

/**
 * �ڲ�����,������ֱ��ʹ��.
 * �� ���ֽ��ַ��� ת��Ϊ ucs-2.
 * @param CharBuf ��ת���������ַ���
 * @param CharBufLen �����ַ������� �ֽ�(1 byte = 8 λ) Ϊ��λ���ַ�����
 * @param WideChar ����Ļ�����
 * @param WideCharBufLen ����������Ĵ�С,�� unicode(ucs-2) Ϊ��λ
 * @param CodeSet ��������ʹ�õ��ַ���.
 * �� CodeSet == NULL ʱʹ�� GB2312,������ĵ��ֽ��ַ�Ϊ GB2312 
 * @return �ɹ����� CharBuf ���� unicode(ucs-2) Ϊ��λ���ַ�����,ʧ�ܷ��ظ���
 * <P>CharBuf ����Ϊ NULL,CharBufLen ����Ϊ 0,����᷵�ش���<BR>
 * WideChar ����Ϊ NULL,WideCharBufLen ����Ϊ 0,����᷵�ش���<BR>
 * CharBuf �� WideChar ָ��Ļ����������ص�,����᷵�ش���</P>
 */
int LC_WideChar2FromChar(const void * CharBuf, const unsigned int CharBufLen,
		void * WideChar, const unsigned int WideCharBufLen,
		const char * CodeSet);
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*/
#endif
/*-------------------------------------------------------------------------*/
