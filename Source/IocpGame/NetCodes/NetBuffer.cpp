// Fill out your copyright notice in the Description page of Project Settings.
#include "NetBuffer.h"
#include "GameTickCounter.h"

NetBuffer::NetBuffer() : CAPACITY((uint32)(MAX_BUFFER_CAPACITY))
{
}

NetBuffer::NetBuffer(uint32 capacity) : CAPACITY(FMath::Clamp<uint32>(capacity, uint32(MIN_BUFFER_CAPACITY), uint32(MAX_BUFFER_CAPACITY)))
{
	if (capacity > uint32(MAX_BUFFER_CAPACITY) || capacity < uint32(MIN_BUFFER_CAPACITY))
		UE_LOG(LogTemp, Warning, TEXT("요청된 버퍼 Capacity가 기준 범위를 벗어납니다."));
}

NetBuffer::~NetBuffer()
{
}

T_BYTE* NetBuffer::GetBuf()
{
	return Buffer;
}

T_BYTE* NetBuffer::GetData()
{
	return Buffer + sizeof(PacketHeader);
}

T_BYTE* NetBuffer::GetWriteCursor()
{
	return Buffer + BufferSize;
}

const uint32 NetBuffer::GetSize() const
{
	return BufferSize;
}

const uint32 NetBuffer::GetCapacity() const
{
	return CAPACITY;
}

const uint32 NetBuffer::GetLeftover() const
{
	return CAPACITY - BufferSize;
}

void NetBuffer::MoveWriteCursor(int32 bytes)
{
	WriteCursor += bytes;
}

void NetBuffer::SetSize(uint32 size)
{
	BufferSize = size;
}

void NetBuffer::Init()
{
	Buffer = new T_BYTE[CAPACITY];
	Clear(); // 완전히 버퍼를 비운다
}

void NetBuffer::Clear()
{
	memset(Buffer, 0, sizeof(Buffer));
	BufferSize = 0;
}

void NetBuffer::Reset()
{
	Clear();
}

bool NetBuffer::Write(T_BYTE* data, uint32 size)
{
	if (BufferSize + size > CAPACITY)
	{
		/////////////// TESTING
		for (int i = 0; i < (int)size; ++i)
		{
			UE_LOG(LogTemp, Error, TEXT("버퍼 %i번 문자: %i"), i, data[i]);
		}
		UE_LOG(LogTemp, Error, TEXT("버퍼 크기를 초과해 Write할 수 없습니다!"));
		return false;
	}
	memcpy(GetWriteCursor(), data, size);
	BufferSize += size;
	return true;
}


/**
* SendBuffer
*/
void SendBuffer::Reset()
{
	NetBuffer::Reset();
	SetDefaultHeader();
}

void SendBuffer::Init()
{
	NetBuffer::Init();
	SetDefaultHeader();
}

bool SendBuffer::Write(T_BYTE* data, uint32 size)
{
	if (!NetBuffer::Write(data, size)) return false;
	((PacketHeader*)Buffer)->size += size;
	return true;
}

void SendBuffer::SetDefaultHeader()
{
	// 버퍼가 비었다고 가정
	// 여기서 정의된 헤더값들은 유효하지 않기 때문에 반드시 발송 전에 적절한 헤더 값으로 교체해 주는 작업이 필요
	// 단 size의 경우에는 Write() 과정에서 알아서 업데이트 되기 때문에 senderType과 id만 제대로 입력해주면 됨
	((PacketHeader*)Buffer)->size = sizeof(PacketHeader);
	((PacketHeader*)Buffer)->senderType = HostTypeEnum::NONE;
	((PacketHeader*)Buffer)->senderId = 0;
	((PacketHeader*)Buffer)->protocol = PacketProtocol::NO_PROTOCOL;
	((PacketHeader*)Buffer)->id = PacketId::DEFAULT;

	BufferSize = sizeof(PacketHeader); // += 가 아니라 = 임에 유의
}


/**
* RecvBuffer
*/
